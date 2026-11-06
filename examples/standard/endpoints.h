#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include "response.h"
#include "request.h"
#include "consts.h"
#include "defineendpoints.h"

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
