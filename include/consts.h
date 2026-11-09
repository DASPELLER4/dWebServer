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

#ifndef TIMEOUT
	#define TIMEOUT -1
#endif

#ifndef MAXMEMORYUSE
	#define MAXMEMORYUSE 10000000
#endif

#endif
