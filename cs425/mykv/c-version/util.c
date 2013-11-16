/**
 * util.c
 *
 * Util implementation
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define DEFAULT_INPUT_LENGTH 128

char* readLine(FILE* fp)
{
  char* ret = NULL;
  int size = DEFAULT_INPUT_LENGTH;
  int count = 0;
  char c;

  if(fp == NULL)
    return NULL;

  if(feof(fp))
    return NULL;

  ret = malloc(size*sizeof(char));
  memset(ret, 0, size);

  while((c = fgetc(fp)) != '\n' && !feof(fp)) {
    ret[count++] = c;
    if(count >= size) {
      size *= 2;
      ret = realloc(ret, size*sizeof(char));
    }
  }

  return ret;
}

const char* stristr(const char* str, const char* match)
{
  char* x = NULL;
  char* y = NULL;
  char* pos = NULL;
  int i = 0, offset = -1, mLen = 0, sLen = 0;

  if(match == NULL || str == NULL)
    return NULL;

  sLen = strlen(str);
  mLen = strlen(match);

  if(mLen > sLen)
    return NULL; /** Cannot find a substr which is larger than original */

  x = malloc((sLen+1) * sizeof(char));
  y = malloc((mLen+1) * sizeof(char));

  strncpy(x, str, sLen);
  strncpy(y, match, mLen);

  /** Better safe than sorry - null terminate our strings */
  x[sLen] = '\0';
  y[mLen] = '\0';

  for(i = 0 ; i < sLen ; i++) {
    x[i] = tolower(str[i]);
    if(i < mLen)
      y[i] = tolower(match[i]);
  }

  pos = strstr(x, y);
  offset = (pos == NULL) ? -1 : pos - x;
  
  free(x);
  free(y);

  return (offset == -1) ? NULL : str + offset;
}

char* extractData(const char* str)
{
  int sLen    = strlen(str);
  char* orig  = malloc((sLen+1) * sizeof(char));
  char* copy  = orig;
  char* start = NULL;
  char* ret   = NULL;

  /** Better safe than sorry - null terminate */
  strncpy(copy, str, sLen);
  copy[sLen] = '\0';

  /** Open quote */
  while(*copy != '"') {
    if(*copy == '\0') {
      free(orig);
      return NULL;
    }
    copy++;
  }

  start = ++copy;

  /** Close quote */
  while(*copy != '"') {
    if(*copy == '\0') {
      free(orig);
      return NULL;
    }
    copy++;
  }

  *copy = '\0';
  
  ret = malloc((strlen(start)+1) * sizeof(char));
  strncpy(ret, start, strlen(start));

  /* Make sure anything we return is null-terminated */
  ret[strlen(start)] = '\0';

  free(orig);

  return ret;
}


