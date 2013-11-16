/**
 * util_tests.h
 *
 * All the test cases for util
 */

#ifndef TESTS_UTIL_TESTS_H__
#define TESTS_UTIL_TESTS_H__

#include "testing.h"
#include "../util.h"

static int testStristr(char* msg)
{
  char x[30] = { 'T', '3', 's', 'T', '\0' };
  char y[30] = { 't', '3', 's', 't', '\0' };
  char z[30] = { 't', 'e', 's', 't', '\0' };
  const char* str = NULL;

  if(stristr(x, y) == NULL) {
    fail(msg, "Two equal strings could not be matched");
    return 0;
  }

  if((str = stristr(x, z)) != NULL) {
    fail(msg, "Matched two unequal strings");
    return 0;
  }

  return 1;
}

static int testSkipwhitespace(char* msg)
{
  char x[30] = "     Whitspa ce";
  char y[30] = "No beginning white spac e";

  if(skipWhitespace(x) != (x+5)) {
    fail(msg, "Failed to skip beginning whitespace");
    return 0;
  }

  if(skipWhitespace(y) != y) {
    fail(msg, "Skipped non-existent whitespace");
    return 0;
  }

  return 1; 
}

static int testExtractData(char* msg)
{
  char x[40] = "Try to \"extractThis\" data";
  char y[40] = "There is nothing to\" extract";
  char* data = NULL;

  if((data = extractData(x)) != NULL) {
    if(strncmp(data, "extractThis", strlen("extractThis"))) {
      fail(msg, "Did not appropriately extract data");
      free(data);
      return 0;
    }
    free(data);
  } else {
    fail(msg, "Could not extract data");
    return 0;
  }

  if(extractData(y) != NULL) {
    fail(msg, "Tried to extract non-existent data");
    return 0;
  }

  return 1;
}

static int testReadLine(char* msg)
{
  FILE* fp = fopen("test1.txt", "r");
  int lineNo = 1;
  char* line = NULL;

  if(fp == NULL) { /** Try 2 locations then fail */
    fp = fopen("tests/test1.txt", "r");
    if(fp == NULL) {
      fail(msg, "Could not open file!");
      return 0;
    }
  }

  while(!feof(fp)) {
    line = readLine(fp);
    if(lineNo == 1) {
      if(strncmp(line, "Test this 123 line", strlen("Test this 123 line"))) {
        fail(msg, "Did not receive the correct line input when reading file");
        free(line);
        fclose(fp);
        return 0;
      }
    }
    free(line);
    lineNo++;
  }

  fclose(fp);

  return 1;
}

#endif /** TESTS_UTIL_TESTS_H__ */

