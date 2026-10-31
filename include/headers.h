#ifndef HEADERS_H
#define HEADERS_H

#include "keyvalues.h"

typedef struct{
	int count;
	int size;
	keyvalue_t *headers;
} header_t;

int keyHash_(char *data);
header_t *newHeaders();
keyvalue_t parseHeader(char *);
void *appendHeader(header_t *, keyvalue_t);

#endif
