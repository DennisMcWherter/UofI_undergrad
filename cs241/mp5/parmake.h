/**
 * parmake.h
 *
 * Basic header
 */

#ifndef PARMAKE_H_
#define PARMAKE_H_

/* Basic call-back functions */
void parsed_command(const char*, const char*);
void parsed_new_key(const char*);
void parsed_dependency(const char*, const char*);

/* Information about each job */
typedef struct _parsed_data_t
{
  char* key;
  char** commands;
  char** deps;
  int numCmds;
  int cmdCnt;
  int numDeps;
  int depsCnt;
} parsed_data_t;

void parsed_data_init(parsed_data_t*);
void parsed_data_destroy(parsed_data_t*);
void parsed_append(const char*, const char*, void(*)(const char*,parsed_data_t*));

void append_data(const char*,int*,int*,char***);
void append_dep(const char*,parsed_data_t*);
void append_cmd(const char*,parsed_data_t*);

void cleanup_this();

#endif /* PARMAKE_H_ */

