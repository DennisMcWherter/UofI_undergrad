/*
 * Machine Problem #7
 * CS 241, Spring 2011
 */

#include "aux_http.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


/*
 *  This function will take in an HTTP request, and will return a hostAddress* containing 
 *  the host name and the port address of the host of the file requested.
 */
hostAddress* getHostFromHTTPRequest(const char *HTTPRequest)
{
    const char sHOST[7] = { 'H', 'o', 's', 't', ':', ' ', '\0'};
    const char sEND[3] = { '\r', '\n', '\0'};

    char *ptr1, *ptr2;

    ptr1 = strstr(HTTPRequest, sHOST);
    if(ptr1) ptr2 = strstr(ptr1, sEND);
    if(ptr1 == NULL) return NULL; 

    ptr1 = ptr1 + 6;
    int hostlen = ptr2 - ptr1;

    hostAddress *host = malloc(sizeof(hostAddress));
    host->hostname = malloc(hostlen + 1);
    strncpy(host->hostname, ptr1, hostlen);
    host->hostname[hostlen] = 0;
   
    host->port = malloc(3);
    strcpy(host->port, "80");

    return host;
}

int cacheHTTPResponse(const char *HTTPResponse){

    if(strstr(HTTPResponse, "max-age=0") == NULL && strstr(HTTPResponse, "no-cache") == NULL && strstr(HTTPResponse, "max-age"))
	return 1;

    return 0;
}

