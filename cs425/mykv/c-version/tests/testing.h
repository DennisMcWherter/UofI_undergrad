/**
 * testing.h
 *
 * Universal information for all test cases
 */

#ifndef TESTS_TESTING_H__
#define TESTS_TESTING_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** Defines */
#define ERRMSG_LEN 0x90 /** Must be > 8 */

/** Important variables/types */
typedef int(*testCase)(char* msg);

/**
 * Structure to hold test cases
 */
typedef struct _test_cases
{
  const char* name;
  testCase test;
} testCases;

/** Internal methods */
/**
 * Generate a failure message
 *
 * @param msg   Message to be altered
 * @param err   Error message to set
 * @return  void
 */
static inline void fail(char* msg, const char* err)
{
  strncpy(msg, err, (ERRMSG_LEN));
}

#endif /** TESTS_TESTING_H__ */

