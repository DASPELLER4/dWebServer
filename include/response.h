#ifndef RESPONSE_H
#define RESPONSE_H

#include "headers.h"

typedef struct{
	int statusCode;
	char *statusText;
	header_t *headers;
	char *body;
	long bodySize;
} response_t;

typedef struct{
	char *data;
	long dataSize;
} compiledResponse_t;

compiledResponse_t compileResponse(response_t);

#endif
