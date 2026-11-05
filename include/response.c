#ifndef RESPONSE_C
#define RESPONSE_C

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "response.h"

#define expandHeaders if(compiledCount>compiledSize-5) compiledHeaders = realloc(compiledHeaders, compiledSize *= 2);

compiledResponse_t compileResponse(response_t response){
	int compiledSize = 100;
	int compiledCount = 0;
	char *compiledHeaders = malloc(compiledSize);
	*compiledHeaders = 0;
	for(int i = 0; i < response.headers->count; i++){
		compiledCount += strlen(response.headers->headers[i].key);
		strcat(compiledHeaders, response.headers->headers[i].key); 
		compiledCount += 2;
		strcat(compiledHeaders, ": ");
		compiledCount += strlen(response.headers->headers[i].value);
		strcat(compiledHeaders, response.headers->headers[i].value);
		compiledCount += 2;
		strcat(compiledHeaders, "\r\n");
	}
	if(response.headers->count < 1)
		strcat(compiledHeaders, "\r\n");
	char *finalResponse = malloc(200 + compiledSize + response.bodySize + 100);
	long offset = snprintf(finalResponse, 200+compiledSize, "HTTP/1.1 %d %s\r\n%s\r\n", response.statusCode, response.statusText, compiledHeaders);
	memcpy(finalResponse + offset, response.body, response.bodySize);
	return (compiledResponse_t){finalResponse, offset + response.bodySize};
}

#endif
