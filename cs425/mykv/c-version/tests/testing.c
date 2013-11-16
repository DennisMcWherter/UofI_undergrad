/**
 * testing.c
 *
 * Unit testing for different modules
 *
 */

#include <stdio.h>

#include "testing.h"
#include "config_tests.h"
#include "memdb_tests.h"
#include "util_tests.h"
#include "sockets_tests.h"

testCases tests[] = {
  {"testStristr", testStristr},
  {"testSkipWhitespace", testSkipwhitespace},
  {"testExtractData", testExtractData},
  {"testReadLine", testReadLine},
  {"testInitServerConfig", testInitServerConfig},
  {"testDestroyServerConfig", testDestroyServerConfig},
  {"testLoadConfiguration", testLoadConfiguration},
  /* {"testServer", testServer} */
  {"testEndianess", testEndianess},
  {"testSwitchEndianess", testSwitchEndian},
  {"testMemDB", testMemDB}
};

testCases threadedTests[] = {
  {"testMemDBThreaded", testMemDBThreaded}
};

int main(int argc)
{
  char err[ERRMSG_LEN];
  int numTests = sizeof(tests) / sizeof(testCases);
  int i = 0;
  int result = 0;
  int success = 0;
  float sRate = 0.f;
  int numThreadedTests = sizeof(threadedTests) / sizeof(testCases);

  printf("====================\nStandard Tests\n====================\n");

  for(i = 0 ; i < numTests ; ++i) {
    printf("[%d/%d] Running %s... ", (i+1), numTests, tests[i].name);
    result = tests[i].test(err);
    if(result == 0) {
      printf("Failed: %s\n", err);
    } else {
      printf("Success!\n");
      success++;
    }
  }

  sRate = ((float)success / (float)numTests) * 100.f;

  printf("Results: %d out of %d tests (%.2lf%%) succeeded\n", success, numTests, sRate);

  if(argc < 2) { /* Run threaded */
    printf("====================\nThreaded Tests\n====================\n");
    success = 0;

    for(i = 0 ; i < numThreadedTests ; ++i) {
      printf("[%d/%d] Running %s...", (i+1), numThreadedTests, threadedTests[i].name);
      if(result == 0) {
        printf("Failed: %s\n", err);
      } else {
        printf("Success!\n");
        success++;
      }
    }

    sRate = ((float)success / (float)numThreadedTests) * 100.f;

    printf("Results: %d out of %d threaded tests (%.2lf%%) succeeded\n", success, numThreadedTests, sRate);
  }
  
  return 0;
}

