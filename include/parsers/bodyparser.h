#ifndef BODYPARSER_H
#define BODYPARSER_H

#include <stdbool.h>

#include "request.h"
#include "headers.h"
#include "keyvalues.h"

typedef struct{
	int size;
	int count;
	keyvalue_t *params; 
	char *value;
	int length;
} parsedData_t;

typedef struct{
	int size;
	int count;
	parsedData_t *data;
} parsedBody_t;

int isLineBoundary(char*, char*);
parsedBody_t parseMultiFormData(request_t);
parsedData_t *getField(parsedBody_t, char*);

#endif
