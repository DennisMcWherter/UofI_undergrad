/*
  CS241 MP1: Part 2 Testing Program
  Andrew Zich - 2011
  azich2@illinois.edu

  Save as test2.c in mp1 directory

Compile using: gcc -g -o test2 test2.c libdictionary/libdictionary.c

Usage: ./test2 [options]
  Use any combination of the following:
    -i: Initialize the dictionary
    -a [key] [value]: Add a key/value to the dictionary
    -p [string]: Parse a string into the dictionary
    -g [key]: Get a value by its key from the dictionary
    -r [key]: Remove a key from the dictionary
    -d: Destroy the dictionary

Example usage: ./test2 -i -a "key" "value" -a "key2" "value" -a "key3" "value" -a "key" "value2" -r "key3" -g "non-existant" -g "key" -p "key3: value" -p "bad key-value" -g "key3" -d

Example (correct) output:
  Running init()
    completed
  Running add("key","value")
    returned 0
    key was NOT modified
    value was NOT modified
  Running add("key2","value")
    returned 0
    key was NOT modified
    value was NOT modified
  Running add("key3","value")
    returned 0
    key was NOT modified
    value was NOT modified
  Running add("key","value2")
    returned KEY_EXISTS
    key was NOT modified
    value was NOT modified
  Running remove("key3")
    returned 0
    remove was NOT modified
  Running get("non-existant")
    returned NULL
    key was NOT modified
  Running get("key")
    returned "value"
    key was NOT modified
  Running parse("key3: value")
    returned 0
    string was MODIFIED
  Running parse("bad key-value")
    returned ILLEGAL_FORMAT
    string was NOT modified
  Running get("key3")
    returned "value"
    key was NOT modified
  Running destroy()
    completed
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libdictionary/libdictionary.h"

char* mstrcpy(const char *s) {
  unsigned long n = strlen(s)+1;
  char *m = malloc(n);
  memcpy(m,s,n);
  return m;
}

void d_retint(const int r) {
  if(r == NO_KEY_EXISTS) {
    printf("  returned NO_KEY_EXISTS\n");
  } else if(r == KEY_EXISTS) {
    printf("  returned KEY_EXISTS\n");
  } else if(r == ILLEGAL_FORMAT) {
    printf("  returned ILLEGAL_FORMAT\n");
  } else {
    printf("  returned %i\n",r);
  }
}

void d_retstr(const char *r) {
  if(r == NULL) {
    printf("  returned NULL\n");
  } else {
    printf("  returned \"%s\"\n",r);
  }
}

void d_compare(const char *name, const char *b, const char *a) {
  if(strcmp(b,a) == 0) {
    printf("  %s was NOT modified\n",name);
  } else {
    printf("  %s was MODIFIED\n",name);
  }
}

int main(int argc, char *argv[]) {
  char *prgm = *argv;
  argv++; argc--;

  dictionary_t dictionary;

  if(argc < 1) {
    printf("Usage: %s [options]\n",prgm);
    printf("Use any combination of the following:\n");
    printf("  -i: Initialize the dictionary\n");
    printf("  -a [key] [value]: Add a key/value to the dictionary\n");
    printf("  -p [string]: Parse a string into the dictionary\n");
    printf("  -g [key]: Get a value by its key from the dictionary\n");
    printf("  -r [key]: Remove a key from the dictionary\n");
    printf("  -d: Destroy the dictionary\n");
  }

  char *key, *value, *string;

  while(argc > 0) {
    if(strcmp(*argv,"-i") == 0) { //Initialize
      printf("Running init()\n");
      dictionary_init(&dictionary);
      printf("  completed\n");
    } else if(strcmp(*argv,"-a") == 0) {
      if(argc < 3) {
	printf("Not enough parameters to -a\n");
	return 2;
      }
      key = mstrcpy(argv[1]);
      value = mstrcpy(argv[2]);
      printf("Running add(\"%s\",\"%s\")\n",argv[1],argv[2]);
      d_retint(dictionary_add(&dictionary, key, value));
      d_compare("key",argv[1],key);
      d_compare("value",argv[2],value);
      argv += 2; argc -= 2;
    } else if(strcmp(*argv,"-p") == 0) {
      if(argc < 2) {
	printf("Not enough parameters to -p\n");
	return 2;
      }
      string = mstrcpy(argv[1]);
      printf("Running parse(\"%s\")\n",argv[1]);
      d_retint(dictionary_parse(&dictionary, string));
      d_compare("string",argv[1],string);
      argv += 1; argc -= 1;
    } else if(strcmp(*argv,"-g") == 0) {
      if(argc < 2) {
	printf("Not enough parameters to -g\n");
	return 2;
      }
      key = mstrcpy(argv[1]);
      printf("Running get(\"%s\")\n",argv[1]);
      d_retstr(dictionary_get(&dictionary, key));
      d_compare("key",argv[1],key);
      argv += 1; argc -= 1;
    } else if(strcmp(*argv,"-r") == 0) {
      if(argc < 2) {
	printf("Not enough parameters to -r\n");
	return 2;
      }
      key = mstrcpy(argv[1]);
      printf("Running remove(\"%s\")\n",argv[1]);
      d_retint(dictionary_remove(&dictionary, key));
      d_compare("remove",argv[1],key);
      argv += 1; argc -= 1;
    } else if(strcmp(*argv,"-d") == 0) {
      printf("Running destroy()\n");
      dictionary_destroy(&dictionary);
      printf("  completed\n");
    } else {
      printf("Did not understand parameter: %s\n",*argv);
      return 3;
    }
    argv++; argc--;
  }
}
