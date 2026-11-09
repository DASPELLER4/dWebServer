#ifndef CONSTS_H
#define CONSTS_H
#if defined __has_include
#if __has_include("userconsts.h")
#include "userconsts.h"
#endif
#endif

#ifndef PORT
	#define PORT 80
#endif

#ifndef ENDPOINTHASHSIZE
	#define ENDPOINTHASHSIZE 256
#endif

#ifndef PREDATASIZE
	#define PREDATASIZE 1000 // :D
#endif

#ifndef MAXTIME
	#define MAXTIME 10 // this sets how long in total we are willing to wait on the client, as opposed to TIMEOUT which waits for a single read
#endif

#ifndef TIMEOUT
	#define TIMEOUT 5000
#endif

#ifndef MAXMEMORYUSE
	#define MAXMEMORYUSE 10000000
#endif

#ifdef STATIC
	#warning "STATIC IS ENABLED!!!!! DO NOT USE THIS OPTION IF YOU EVER INTEND TO READ THE REQUEST BODY OR THE REQUEST HEADER"
#endif

#endif
