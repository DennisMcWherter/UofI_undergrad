#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define MAX_LINE_SIZE 200

static int readLine(FILE *file, char *lineBuf)
{
    int count = 0;
    int c;

    while((c = fgetc(file)) != '\n') {
        if(c == EOF) {
            lineBuf[count] = '\0';
            return (count == 0) ? EOF : count;
        }
        lineBuf[count] = (char) c;
        count++;
    }

    lineBuf[count] = '\0';
    return count;
}

static void alloc_job
(
	char *key,
	char *deps,
	void  (*parsed_new_key)(const char *key),
	void  (*parsed_dependency)(const char *key, const char *dependency)
)
{
    char *token;

	parsed_new_key(key);
    token = strtok(deps, " ");
    while(token != NULL)
	{
		parsed_dependency(key, token);
        token = strtok(NULL, " ");
    }
}

void parser_parse_makefile
(
	const char *makeFileName,
	void  (*parsed_command)(const char *key, const char *command),
	void  (*parsed_new_key)(const char *key),
	void  (*parsed_dependency)(const char *key, const char *dependency)
)
{
    char lineBuf[MAX_LINE_SIZE+1], key[MAX_LINE_SIZE+1];
    char *deps;
    FILE *f;
    int ret;


    f = fopen(makeFileName, "r");
    assert(f != NULL);

    while(readLine(f, lineBuf) != EOF)
	{
        if(isalnum(lineBuf[0]))
		{
            ret = sscanf(lineBuf, "%s", key);
            assert(ret == 1);
            assert(key[strlen(key)-1] == ':');
            deps = lineBuf + strlen(key);
            key[strlen(key)-1] = '\0';
            alloc_job(key, deps, parsed_new_key, parsed_dependency);
        }
		else if(lineBuf[0] == '\t')
			parsed_command(key, lineBuf + 1);
    }

    fclose(f);
}
