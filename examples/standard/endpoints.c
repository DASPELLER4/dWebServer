#ifndef ENDPOINTS_C
#define ENDPOINTS_C

#include <stdio.h>
#include <stdlib.h>

#include "endpoints.h"
#include "bodyparser.h"

response_t root(request_t request){
	printf("/ Accessed\n");
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"}); // keyHash is not necessary for a response
	char *body = "<!DOCTYPE html><body><h1>Hello!</h1><form action=\"formsubmit\" method=\"post\" enctype=\"multipart/form-data\"><input type=\"text\" name=\"username\" placeholder=\"Your name\"><br><textarea name=\"message\" placeholder=\"Your message\" rows=10></textarea><br><input type=\"file\" name=\"file\"><br><input type=\"submit\"></form></body>";
	int len = strlen(body);
	static char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){200, "OK", headers, body, len+1}; // +1 to include the null terminator
}
response_t formsubmit(request_t request){
	printf("/formsubmit Accessed\n");
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"}); // keyHash is not necessary for a response
	parseMultiFormData(request);
	char *body = "<!DOCTYPE html><body><h1>Form Recieved!</h1></body>";
	int len = strlen(body);
	static char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){200, "OK", headers, body, len+1};
}

response_t favicon(request_t request){
	printf("/favicon Accessed\n");
	FILE *fp = fopen("examples/standard/favicon.ico", "r");
	if(!fp){
		printf("Favicon file access failed\n");
		header_t *headers = newHeaders();
		return (response_t){500, "Internal Server Error", headers, NULL, 0};
	}
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	static char d[120] = {0};
	sprintf(d, "%ld", len);
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","image/x-icon"}); // keyHash is not necessary for a response
	appendHeader(headers, (keyvalue_t){0,"Content-Length",d});
	char *body = malloc(len);
	if(!fread(body, len, 1, fp)){
		fclose(fp);
		printf("Favicon file access failed\n");
		header_t *headers = newHeaders();
		return (response_t){500, "Internal Server Error", headers, NULL, 0};
	}
	fclose(fp);
	return (response_t){200, "OK", headers, body, len};
}

response_t fourohfour(request_t request){
	printf("404 Reached\n");
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"});
	char *body = "<!DOCTYPE html><body><h1>404</h1><p>No such page found!!</p></body>";
	int len = strlen(body);
	static char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){404, "Not Found", headers, body, len+1};
}

#endif
