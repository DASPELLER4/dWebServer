#ifndef ENDPOINTS_H
#define ENDPOINTS_H

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

pushEndpoints
#undef endpoints
#define endpoints popEndpoints endpoints addEndpoint(root, "/");
response_t root(request_t);

pushEndpoints
#undef endpoints
#define endpoints popEndpoints endpoints addEndpoint(formsubmit, "/formsubmit");
response_t formsubmit(request_t);

pushEndpoints
#undef endpoints
#define endpoints popEndpoints endpoints addEndpoint(favicon, "/favicon.ico");
response_t favicon(request_t);

#define is404
response_t fourohfour(request_t);

#endif
