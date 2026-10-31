#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include "response.h"
#include "request.h"
#include "consts.h"

extern response_t (*endpointHashMap[ENDPOINTHASHSIZE])(request_t);

void addEndpoint(response_t (*)(request_t), char *);
long pathHash(char *);
response_t (*getEndpoint(char*))(request_t);

#define loadEndpoints\
	addEndpoint(root, "/");

response_t root(request_t);

#endif
