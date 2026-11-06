#ifndef BODYPARSER_H
#define BODYPARSER_H

#include <stdbool.h>

#include "request.h"
#include "headers.h"
#include "keyvalues.h"

typedef struct{
	char *key;
	int size;
	int count;
	keyvalue_t *params; //used for things like content-disposition
	char *value;
} parsedData_t;

typedef struct{
	int size;
	int count;
	parsedData_t *data;
} parsedBody_t;

int isLineBoundary(char*, char*);
parsedBody_t parseMultiFormData(request_t);

#endif
