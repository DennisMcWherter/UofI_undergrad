/* MP2:Simple Shell */

/* LIBRARY SECTION */
#include <ctype.h>              /* Character types                       */
#include <stdio.h>              /* Standard buffered input/output        */
#include <stdlib.h>             /* Standard library functions            */
#include <string.h>             /* String operations                     */
#include <sys/types.h>          /* Data types                            */
#include <sys/wait.h>           /* Declarations for waiting              */
#include <unistd.h>             /* Standard symbolic constants and types */

#include "vector.h"

/* DEFINE SECTION */
#define SHELL_BUFFER_SIZE 256   /* Size of the Shell input buffer        */

/* HELPER METHOD SECTION */
size_t getLine(char* buf,vector_t*,unsigned*);
void clearBuffer(char*,vector_t*,unsigned*);
char** tokenize(char* buf, int * size);
int parseCommand(char* buf, unsigned* count, vector_t* history);
void addAndIncrement(vector_t* history, char* buf, unsigned* count);

/* MAIN PROCEDURE SECTION */
void main()
{
  pid_t x = getpid();
  unsigned y = 1;
  unsigned vec = 0;
  unsigned i = 0;
  char buf[SHELL_BUFFER_SIZE+3];
  int size = 0;
  vector_t history;
  
  vector_init(&history);
  
  while(1) {
    printf("Shell (pid = %d)%d} ", x, y);
    size = getLine(buf, &history, &y);
    if(size < 0)
      printf("Commands should be within the size of 256 characters\n");
    else if(size != 0)
      if(parseCommand(buf, &y, &history))
        break;
  }
  
  /* Free memory */
  vec = vector_size(&history);
  for(i = 0; i < vec ; ++i) {
    free((char*)vector_at(&history, i));
  }
  vector_destroy(&history);
  
} /* end main() */

size_t getLine(char* buf, vector_t* v, unsigned* c)
{
  char* p = 0;
  char* x = 0;
  size_t size = 0;
  fgets(buf, SHELL_BUFFER_SIZE+2, stdin); /* 2 extra bytes for \n and \0 */
  buf[SHELL_BUFFER_SIZE+2] = '\0'; /* Ensure we have a null byte */
  size = strlen(buf);
  if(!(p = strchr(buf, '\n'))) { /* Should work with valid length */
    x = (char*)malloc((SHELL_BUFFER_SIZE+3)*sizeof(char));
    strncpy(x, buf, SHELL_BUFFER_SIZE+2);
    clearBuffer(x, v, c);
    return -1;
  }
  *p = '\0'; /* Clear the new line */
  return size;
}

/* This is used for very long commands ONLY (i.e >SHELL_BUFFER_SIZE) */
void clearBuffer(char* x, vector_t* v, unsigned* c)
{
  /*char c;
  while((c = getchar()) != '\n' && c != EOF);*/
  char * tmp = 0;
  char * p = 0;
  int prev_size = 0;
  int size = strlen(x);
  do
  {
    prev_size = size;
    size *= 2;
    x = realloc(x, size);
    tmp = x+(prev_size-1);
  } while(fgets(tmp, prev_size+1, stdin) && !(p = strchr(x, '\n')));
  (*c)++;
  *p = '\0';
  vector_append(v, x);
}

char** tokenize(char* buf, int * numElems)
{
  char** ret   = 0;
  char* p      = 0;
  char* c      = 0;
  int i = 0;
  *numElems = 0;
  
  if(buf == 0 || *buf == '\0')
    return 0;
  
  c = strndup(buf, strlen(buf)+1);

  p = strtok(c, " ");
  while(p != 0) {
    if(*(p+1) != ' ')
      (*numElems)++;
    p = strtok(0, " ");
  }
  
  free(c);
  
  if(*numElems != 0) {
    ret = (char**)malloc((*numElems)*sizeof(char*));
    memset(ret, 0, sizeof(ret));
  }

  p = strtok(buf, " ");
  while(p != 0) {
    if(*(p+1) != ' ')
      ret[i++] = p;
    p = strtok(0, " ");
  }
  
  return ret;
}

int parseCommand(char* buf, unsigned* count, vector_t* history)
{
  int size = 0;
  int nval = 0;
  unsigned i = 0;
  unsigned vsize = 0;
  char** tokenized = 0;
  char* tmp = 0;
  char* c = strdup(buf);

  tokenized = tokenize(c, &size);
  
  if(tokenized == 0) {
    free(c);
    return 0;
  }
  
  /* Check for relevant built-in commands */
  if(strncmp(tokenized[0], "exit", sizeof("exit")) == 0) {
    free(tokenized);
    free(c);
    return 1;
  } else if(strncmp(tokenized[0], "cd", sizeof("cd")) == 0) {
    if(size > 1 && chdir(tokenized[1]) == 0) {
      /* Add to vector - but first clear whitespace at beginning */
      addAndIncrement(history, buf, count);
    } else {
      printf("Not valid\n");
    }
  } else if(*tokenized[0] == '!') {
    vsize = vector_size(history);
    if(tokenized[0][1] == '#') {
      tmp   = tokenized[0]+2;
      nval  = atoi(tmp);
      if(tokenized[0][2] == '\0') {
        for(i = vsize; i > 0 ; --i) { /* All events */
          printf("!%d: %s\n", vsize-i+1, (char*)vector_at(history, i-1));
        }
      } else if(nval <= 0 || (unsigned)nval > vsize) {
        i = 1; /* Not a valid input don't increment hist */
        printf("Not valid\n");
      } else {
        for(i = vsize; i > (vsize-nval); --i) {
          printf("!%d: %s\n", vsize-i+1, (char*)vector_at(history, i-1));
        }
        i = 0;
      }

      if(i == 0) { /* Add entry and increment counter */
        addAndIncrement(history, buf, count);
      }
    } else {
      /* Check if we need to repeat */
      tmp  = tokenized[0]+1;
      nval = atoi(tmp);
      if(nval > 0 && nval <= vsize) {
        tmp = (char*)vector_at(history, vsize-nval);
        printf("%s\n", tmp);
        parseCommand(tmp, count, history);
      } else {
        printf("Not valid\n");
      }
    }
  } else { /* Non-builtin commands */
    fflush(stdin);
    system(buf);
    addAndIncrement(history, buf, count);
  }

  free(tokenized);
  free(c);
  
  return 0;
}

void addAndIncrement(vector_t* history, char* buf, unsigned* count)
{
  int i = 0;
  char* entry = 0;
  if(buf == 0 || history == 0 || count == 0)
    return;

  while(*buf == ' ') {
    buf++;
    i++;
  }

  if(i == strlen(buf))
    return;

  entry = strndup(buf, strlen(buf)+1);

  vector_append(history, entry);
  (*count)++;
}

