/**
 * util.h
 *
 * General methods which may be useful for multiple files
 */

#ifndef UTIL_H__
#define UTIL_H__

#include <stdio.h>
#include <string.h>

/**
 * Read a line from a file
 * NOTE: This function allocates memory and must
 *  be freed by the caller.
 *
 * @param fp  File pointer to be read
 * @return  A the line reader or NULL if end of file.
 */
char* readLine(FILE* fp);

/**
 * Case insensitive strstr()
 *
 * @param match   String to match
 * @param str     Original string
 * @return  Same as strstr()
 */
const char* stristr(const char* match, const char* str);

/**
 * Extract data between quotes
 *  NOTE: This method allocates memory
 *   which must be freed by the caller.
 *
 * @param str   Input string
 * @return  A copy of the data. Returns NULL
 *  if no quotes or improper quotes.
 */
char* extractData(const char* str);

/**
 * Eat whitespace
 *
 * @param str   String to skip whitespace on
 * @return  A pointer to the string with skipped whitepsace.
 *  Returns NULL if str == NULL or entire line is blank
 */
static const char* skipWhitespace(const char* str)
{
  if(str == NULL)
    return NULL;

  while(*str == ' ' || *str == '\t') {
    if(*str == '\0')
      return NULL;
    str++;
  }

  return str;
}

#endif /** UTIL_H__ */

