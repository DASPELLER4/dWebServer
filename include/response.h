#ifndef RESPONSE_H
#define RESPONSE_H

#include "headers.h"

typedef struct{
	int statusCode;
	char *statusText;
	header_t *headers;
	char *body;
	int responseSize; // may rework this into a better mechanism
} response_t;

#endif
