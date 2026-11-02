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
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"}); // keyHash is not necessary for a response
	char *body = "<!DOCTYPE html><body><h1>Hello!</h1><form action=\"fileupload\" method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"file\"><input type=\"submit\"></form></body>";
	int len = strlen(body);
	char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){200, "OK", headers, body, len+1}; // +1 to include the null terminator
}

response_t fupload(request_t request){
	printf("/fileupload Accessed\n");
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"}); // keyHash is not necessary for a response
	char *body = "<!DOCTYPE html><body><h1>File Uploaded!</h1></body>";
	int len = strlen(body);
	char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){200, "OK", headers, body, len+1};
}

response_t favicon(request_t request){
	printf("/favicon Accessed\n");
	FILE *fp = fopen("include/favicon.ico", "r");
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char d[120] = {0};
	sprintf(d, "%ld", len);
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","image/x-icon"}); // keyHash is not necessary for a response
	appendHeader(headers, (keyvalue_t){0,"Content-Length",d});
	char *body = malloc(len);
	fread(body, len, 1, fp);
	fclose(fp);
	return (response_t){200, "OK", headers, body, len};
}

#endif
