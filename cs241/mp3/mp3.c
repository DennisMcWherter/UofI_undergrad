/*
* Machine Problem #3
* CS 241,Fall 2011
*
* Implements parallel file-based mergesort 
*    Uses qsort library for per file sort
*/


#include <pthread.h>			/* Thread related functions				 */
#include <stdio.h>              /* Standard buffered input/output        */
#include <stdlib.h>             /* Standard library functions            */
#include <string.h>             /* String operations                     */

void* mergeDelegate(void* data);
void* mergeThread(void* data);
void* sortThread(void* data);
int compare(const void* a, const void* b);
int* getArray(char* filename, long* lines);
void writeSorted(const char* filename, int* array, long lines);
void writeSortedFile(FILE* file);
long countLines(FILE* file);
FILE* mergeSorted(FILE* file1, FILE* file2);
void treeMerge(FILE** sorted, int num);

typedef struct _merge_files
{
  FILE* file1;
  FILE* file2;
} merge_files_t;

typedef struct _tree_files
{
  FILE** files;
  int count;
} tree_files_t;

/* MAIN PROCEDURE SECTION */
int main(int argc, char **argv)
{
  pthread_t * threads = 0;
  FILE** sortedFiles = 0;
  int i = 0;
  int result = 0;
  int numThreads = 0;
  pthread_attr_t attr;
  tree_files_t tree;

	if(argc <= 1) {
    fprintf(stderr, "Invalid usage: ./mp3 arg1 arg2 arg3 ... argn\n");
    return 1;
  }

  /* Need a dynamic array of threads */
  numThreads = argc-1;
  threads = (pthread_t*)malloc(numThreads*sizeof(pthread_t));
  sortedFiles = (FILE**)malloc(numThreads*sizeof(FILE*));
  memset(sortedFiles, 0, numThreads*sizeof(FILE*));

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Sort */
  for(i = 0; i < numThreads; ++i) {
    result = pthread_create(&threads[i], &attr, sortThread, (void*)argv[i+1]);
    if(result) {
      fprintf(stderr, "Error: pthread_create() result: %d\n", result);
      exit(-1);
    }
  }
  for(i = 0; i < numThreads; ++i) {
    result = pthread_join(threads[i], (void**)&sortedFiles[i]);
    if(result) {
      fprintf(stderr, "Error: pthread_join() result: %d\n", result);
      exit(-1);
    }
  }

  /* Merge */
  tree.files = sortedFiles;
  tree.count = numThreads;
  pthread_create(&threads[0], &attr, mergeDelegate, (void*)&tree);
  pthread_join(threads[0], 0);
  
  /* Clean up */
  pthread_attr_destroy(&attr);
  free(threads);
  for(i = 0; i < numThreads; ++i) {
    if(sortedFiles[i] != 0)
      fclose(sortedFiles[i]);
  }
  free(sortedFiles);

  /* pthread_exit(0)? */
	return 0;
} /* end main() */

void* sortThread(void* data)
{
  FILE* sFile = 0;
  int* arr = 0;
  const char* suffix = ".sorted";
  char* name = (char*)data;
  const int size  = strlen(name)+1; /* +1 for null byte */
  const int ssize = strlen(suffix); /* strncat will move the null byte */
  char* sorted = (char*)malloc((size+ssize)*sizeof(char)); /* Freed in main */
  long lines = 0;

  /* New filename of *.sorted */
  strncpy(sorted, name, size);
  if(strncmp(sorted, name, size) != 0) {
    fprintf(stderr, "Did not copy strings correctly!\n");
    exit(-1);
  }
  strncat(sorted, suffix, ssize);

  /* Sort everything */
  arr = getArray(name, &lines);

  if(arr == 0) {
    free(sorted);
    pthread_exit(0);
  }

  qsort(arr, lines, sizeof(int), compare);

  /* Write *.sorted */
  writeSorted(sorted, arr, lines);
  printf("This worker thread writes %ld lines to \"%s\".\n", lines, sorted);

  sFile = fopen(sorted, "r");

  free(arr);
  free(sorted);

  pthread_exit((void*)sFile);
}

void* mergeDelegate(void* data)
{
  tree_files_t * tree = (tree_files_t*)data;
  treeMerge(tree->files, tree->count);
  pthread_exit(0);
}

void* mergeThread(void* data)
{
  merge_files_t * merged = (merge_files_t*)data;
  FILE* file = mergeSorted(merged->file1, merged->file2);
  pthread_exit((void*)file);
}

int* getArray(char* filename, long* lines)
{
  ssize_t read = 0;
  size_t length = 255;
  char * c = (char*)malloc(256*sizeof(char));
  FILE* file = 0;
  int size = 16;
  int* array = (int*)malloc(size*sizeof(int));

  if(filename == 0)
    return 0;
  
  if((file = fopen(filename, "r")) == 0) {
    fprintf(stderr, "Could not open file: %s\n", filename);
    exit(-1);
  }

  while((read = getline(&c, &length, file)) != EOF) {
    if(read > 1) {
      if(*lines >= size) { /* Resize if needed */
        size *= 2;
        array = (int*)realloc(array, size*sizeof(int));
      }
      array[*lines] = atoi(c);
      (*lines)++;
    }
  }

  fclose(file);
  free(c);

  return array;
}

FILE* mergeSorted(FILE* file1, FILE* file2)
{
  FILE* tmp = tmpfile();
  ssize_t read1 = 0, read2 = 0;
  long a = 0, b = 0;
  long f1pos = 0, f2pos = 0;
  size_t length = 255;
  char* c1 = (char*)malloc(256*sizeof(char));
  char* c2 = (char*)malloc(256*sizeof(char));
  
  rewind(file1);
  rewind(file2);

  f1pos = ftell(file1);
  f2pos = ftell(file2);
  while((read1 = getline(&c1, &length, file1)) != EOF && (read2 = getline(&c2, &length, file2)) != EOF) {
    if(read1 > 1 && read2 > 1) {
      a = atoi(c1);
      b = atoi(c2);
      if(a == b) {
        fprintf(tmp, "%ld\n", a);
      } else if(a < b) {
        fprintf(tmp, "%ld\n", a);
        fseek(file2, f2pos, SEEK_SET);
      } else {
        fprintf(tmp, "%ld\n", b);
        fseek(file1, f1pos, SEEK_SET);
      }
    } else if(read1 > 1 && read2 <= 1) {
      fseek(file1, f1pos, SEEK_SET); /* Rewind file1 */
    } else if(read1 <= 1 && read2 > 1) {
      fseek(file2, f2pos, SEEK_SET); /* Rewind file2 */
    }
		f1pos = ftell(file1);
		f2pos = ftell(file2);
  }
  /* Append the remaining to EOF */
	if(read2 == EOF && read1 != EOF) {
		fseek(file1, f1pos, SEEK_SET);
	} else if(read1 == EOF && read2 != EOF) {
		fseek(file2, f2pos, SEEK_SET);
	}

  while((read2 = getline(&c2, &length, file2)) != EOF){
    if(read2 > 1) {
      a = atoi(c2);
      fprintf(tmp, "%ld\n", a);
    }
	}
  while((read1 = getline(&c1, &length, file1)) != EOF){
    if(read1 > 1) {
      a = atoi(c1);
      fprintf(tmp, "%ld\n", a);
    }
  }
  
  rewind(file1);
  rewind(file2);
  
  fprintf(stdout, "Merged %ld lines and %ld lines into %ld lines.\n", countLines(file1), countLines(file2), countLines(tmp));

	rewind(tmp);
  
  free(c1);
  free(c2);
  
  return tmp;
}
          
long countLines(FILE* file)
{
  ssize_t read = 0;
  long count = 0;
  size_t length = 255;
  char* c = (char*)malloc(256*sizeof(char));
  
  rewind(file); /* Rewind before count */
  
  while((read = getline(&c, &length, file)) != EOF) {
    if(read > 1) {
      count++;
    }
  }
  
  free(c);
  rewind(file); /* Rewind before return */
  return count;
}

void writeSorted(const char* filename, int* arr, long length)
{
  long i = 0;
  FILE* file = fopen(filename, "w");

  for(i = 0; i < length; ++i)
    fprintf(file, "%d\n", arr[i]);

  fclose(file);
}

void writeSortedFile(FILE* file)
{
  ssize_t read = 0;
  size_t length = 255;
  char* c = (char*)malloc(256*sizeof(char));
  FILE* final = fopen("sorted.txt", "w+");
  int x = 0;
  
  while((read = getline(&c, &length, file)) != EOF) {
    if(read > 1) {
      x = atoi(c);
      fprintf(final, "%d\n", x);
    }
  }
  fclose(final);
  free(c);
}

int compare(const void* a, const void* b)
{
  return (*(int*)a) - (*(int*)b);
}

void treeMerge(FILE** sortedFiles, int num)
{
  int i = 0;
  int mergesLeft = num;
  int merged = 0;
  int tmp = 0;
  pthread_t * threads = (pthread_t*)malloc(num*sizeof(pthread_t));
  merge_files_t * merges = (merge_files_t*)malloc(num*sizeof(merge_files_t));
  FILE** mergeQueue = (FILE**)malloc((num+1)*sizeof(FILE*));
  FILE** tmpFiles = (FILE**)malloc(2*num*sizeof(FILE*));
  memset(mergeQueue, 0, (num+1)*sizeof(FILE**));
  memset(tmpFiles, 0, 2*num*sizeof(FILE**));
  
  for(i = 0; i < num; ++i)
    mergeQueue[i] = sortedFiles[i];

  /* Each level should be floor(_num_of_threads/2) */
	while(mergeQueue[1] != 0)//mergeQueue[1] != 0)  
	{
		merged = 0;
		while(mergeQueue[1] != 0) {
      merges[merged].file1 = mergeQueue[0];
      merges[merged].file2 = mergeQueue[1];
      pthread_create(&threads[merged], 0, mergeThread, (void*)&merges[merged]);

			/* Down-shift queue */
			for(i = 0; i < num; ++i) {
				if(i+2 < num)
					mergeQueue[i] = mergeQueue[i+2];
				else
					mergeQueue[i] = 0;
			}

     /* Sanity check - paranoia. Make sure to maintain the queue */
			for(i = num; i > 0; --i) {
				if(mergeQueue[i] != 0 && mergeQueue[i-1] == 0) {
					mergeQueue[i-1] = mergeQueue[i];
					mergeQueue[i] = 0;
					i = num+1; /* This is hacky, but check everything is in place */
				}
			}

			mergesLeft -= 2;
			merged++;
    }

    if(mergeQueue[0] != 0 && mergesLeft > 0) /* Merge right to left so move this */
      mergeQueue[merged] = mergeQueue[0];

		for(i = 0; i < merged; ++i) {
			pthread_join(threads[i], (void**)&mergeQueue[i]);
			tmpFiles[tmp] = mergeQueue[i];
			tmp++;
			mergesLeft++;
		}
  }
  
  writeSortedFile(mergeQueue[0]);
  
  free(threads);
  free(merges);
  free(mergeQueue);
  
  for(i = 0; i < 2*num; ++i) {
    if(tmpFiles[i] != 0)
      fclose(tmpFiles[i]);
  }
  free(tmpFiles);
}

