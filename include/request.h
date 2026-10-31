#ifndef REQUEST_H
#define REQUEST_H

#include "headers.h"

typedef struct {
	char *type;
	char *path;
	header_t *headers;
	char *body;
} request_t;

#endif
