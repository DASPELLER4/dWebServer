#ifndef DEFINEENDPOINTS_H
#define DEFINEENDPOINTS_H

#include <string.h>

#include "response.h"
#include "request.h"
#include "consts.h"

#define pushEndpoints _Pragma("push_macro(\"endpoints\")")
#define popEndpoints _Pragma("pop_macro(\"endpoints\")")

#ifdef DEBUG
#define endpoints printf("Loading endpoints.\n");
#else
#define endpoints
#endif

extern response_t (*endpointHashMap[ENDPOINTHASHSIZE])(request_t);

void addEndpoint(response_t (*)(request_t), char *);
long pathHash(char *);
response_t (*getEndpoint(char*))(request_t);

#endif
