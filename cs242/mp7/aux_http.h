/*
 * Machine Problem #7
 * CS 241, Spring 2011
 */

#ifndef AUX_HTTP_H_
#define AUX_HTTP_H_

typedef struct _host{
	char *hostname;
	char *port;
} hostAddress;

hostAddress* getHostFromHTTPRequest(const char *HTTPRequest);
int cacheHTTPResponse(const char *HTTPResponse);

#endif /* AUX_HTTP_H_ */
