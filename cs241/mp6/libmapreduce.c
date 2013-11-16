/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>

#include "libmapreduce.h"
#include "libdictionary.h"

typedef struct _thread_data {
  int count;
  int* pipes;
  mapreduce_t* mr;
} thread_data_t;

static const int BUFFER_SIZE = 2048;

static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{
  const char* newval = value;
  const char* val = NULL;
  if((val = dictionary_get(&mr->dict, key))) {
    newval = mr->reduce(value, val);
    dictionary_remove_free(&mr->dict, key);
    free((void*)value);
  }
  dictionary_add(&mr->dict, key, newval);
}


static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)
{
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	buffer[offset + bytes_read] = '\0';

	if (bytes_read == 0)
		return 0;

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		//assert(split != NULL);

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
		buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}


void mapreduce_init(mapreduce_t *mr,
	                void (*mymap)(int, const char *),
					const char *(*myreduce)(const char *, const char *))
{
  dictionary_init(&mr->dict);
  mr->map    = mymap;
  mr->reduce = myreduce;
}

void* read_thread(void* dat)
{
  int i = 0;
  int j = 0;
  thread_data_t* data = (thread_data_t*)dat;
  int c = data->count;
  int* pipes = data->pipes;
  mapreduce_t* mr = data->mr;
  struct pollfd* x = (struct pollfd*)malloc(c*sizeof(struct pollfd));
  char** buf = (char**)malloc(c*sizeof(char*));

  for(i = 0 ; i < c ; ++i) {
    x[i].fd = pipes[i];
    x[i].events = POLLIN;
    buf[i] = (char*)malloc((BUFFER_SIZE+1)*sizeof(char));
    buf[i][0] = '\0';
  }

  i = 0;
  while(i < c) {
    poll(x, c, -1);
    for(j = 0 ; j < c ; ++j) {
      if(x[j].revents & POLLIN) {
        while(read_from_fd(x[j].fd, buf[j], mr));
        i++;
      }
    }
  }

  for(i = 0 ; i < c ; ++i)
    free(buf[i]);

  free(x);
  free(buf);
  free(pipes);
  free(data);
  return NULL;
}

void mapreduce_map_all(mapreduce_t *mr, const char **values)
{
  int i = 0;
  int c = 0;
  int pipes[2];
  thread_data_t* data = (thread_data_t*)malloc(sizeof(thread_data_t));

  if(values == NULL)
    return; /* Simple error checking */

  /* Apologies for my laziness, this is extra work. But it's not production */
  for(c = 0 ; values[c] != '\0' ; ++c);
  
  data->mr    = mr;
  data->count = c;
  data->pipes = (int*)malloc(c*sizeof(int));

  for(i = 0 ; i < c ; ++i) {
    pipe(pipes);
    if(!fork()) { /* Child */
      close(pipes[0]); /* No need to read */
      mr->map(pipes[1], values[i]);
      free(data->pipes);
      free(data);
      exit(0);
    } else { /* Parent */
      close(pipes[1]); /* No need to write */
      data->pipes[i] = pipes[0];
    }
  }

  pthread_create(&mr->thread, NULL, read_thread, (void*)data);
}

void mapreduce_reduce_all(mapreduce_t *mr)
{
  pthread_join(mr->thread, NULL);
}

const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{
	return dictionary_get(&mr->dict, result_key);
}

void mapreduce_destroy(mapreduce_t *mr)
{
  dictionary_destroy_free(&mr->dict);
}

