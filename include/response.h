#ifndef RESPONSE_H
#define RESPONSE_H

#include "headers.h"

typedef struct{
	int statusCode;
	header_t *headers;
	char *content;
} response_t;

#endif
