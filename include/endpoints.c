#ifndef ENDPOINTS_C
#define ENDPOINTS_C

#include <stdio.h>
#include <stdlib.h>

#include "endpoints.h"

response_t (*endpointHashMap[ENDPOINTHASHSIZE])(request_t);

void addEndpoint(response_t (*endpoint)(request_t), char *path){
	long hash = pathHash(path);
	if(endpointHashMap[hash]){
		fprintf(stderr,"Hash collision, try increasing ENDPOINTHASHSIZE");
		exit(1);
	}
	endpointHashMap[hash] = endpoint;

}

long pathHash(char *path){
	long hash = 0;
	for(;*path;path++)
		hash += (*path^0x9a)*23;
	return hash % ENDPOINTHASHSIZE;
}

response_t (*getEndpoint(char *path))(request_t){
	int end = -1;
	char temp = 0;
	for(int i = 0; path[i]; i++){
		if(path[i] == '?'){
			end = i;
			path[i] = 0;
			temp = '?';
			break;
		}
		if(path[i] == '#'){
			end = i;
			path[i] = 0;
			temp = '#';
			break;
		}
	}
	long hash = pathHash(path);
	if(end != -1)
		path[end] = temp;
	return endpointHashMap[hash];
}

response_t root(request_t request){
	printf("/ Accessed\n");
}

#endif
