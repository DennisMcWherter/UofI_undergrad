#ifndef __PARSER_H
#define __PARSER_H

void parser_parse_makefile
(
	const char *,
	void (*parsed_command)   (const char *, const char *),
	void (*parsed_new_key)   (const char *),
	void (*parsed_dependency)(const char *, const char *)
);

#endif
