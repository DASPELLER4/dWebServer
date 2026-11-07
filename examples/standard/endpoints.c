#ifndef ENDPOINTS_C
#define ENDPOINTS_C

#include <stdio.h>
#include <stdlib.h>

#include "endpoints.h"
#include "bodyparser.h"
#include "utils.h"

void initwebsite(){
	FILE *fp = fopen("examples/standard/messagedata", "a");
	if(!fp){
		fprintf(stderr, "Couldn't open messagedata file\n");
		exit(1);
	}
	fclose(fp);

}

response_t root(request_t request){
	printf("/ Accessed\n");
	header_t *headers = newHeaders();
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"}); // keyHash is not necessary for a response
	FILE *fp = fopen("examples/standard/messagedata", "r");
	if(!fp){
		header_t *headers = newHeaders();
		return (response_t){500, "Internal Server Error", headers, NULL, 0};
	}
	fseek(fp, 0, SEEK_END);
	int dataLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *allMessageData = malloc(dataLen);
	if(dataLen && !fread(allMessageData, dataLen, 1, fp)){
		fclose(fp);
		header_t *headers = newHeaders();
		return (response_t){500, "Internal Server Error", headers, NULL, 0};
	}
	fclose(fp);
	char *body = malloc(1000+dataLen);
	int len = snprintf(body, 999+dataLen, "<!DOCTYPE html><head><style>table,th,td{border: 1px solid black;}</style></head><body><h1>Hello!</h1><form action=\"formsubmit\" method=\"post\" enctype=\"multipart/form-data\"><input type=\"text\" name=\"username\" placeholder=\"Your name\"><br><textarea name=\"message\" placeholder=\"Your message\" rows=10></textarea><br><input type=\"file\" name=\"file\"><br><input type=\"submit\"></form><br><table><tr><td>Username</td><td>Message</td></tr>%s</table></body>", allMessageData);
	static char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){200, "OK", headers, body, len+1}; // +1 to include the null terminator
}
response_t formsubmit(request_t request){
	printf("/formsubmit Accessed\n");
	header_t *headers = newHeaders();
	parsedBody_t parsedBody = parseMultiFormData(request);
	parsedData_t *file = getFieldByName(parsedBody, "file");
	parsedData_t *username = getFieldByName(parsedBody, "username");
	parsedData_t *message = getFieldByName(parsedBody, "message");
	if(!username || !message){
		header_t *headers = newHeaders();
		return (response_t){500, "Internal Server Error", headers, NULL, 0};
	}
	HTMLEscape(message);
	HTMLEscape(username);
	appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"}); // keyHash is not necessary for a response
	char *body = malloc(1000 + 8 + 9 + 10 + username->length + message->length);
	FILE *fp = fopen("examples/standard/messagedata", "a");
	fwrite("<tr><td>", 1, 8, fp);
	fwrite(username->value, 1, username->length, fp);
	fwrite("</td><td>", 1, 9, fp);
	fwrite(message->value, 1, message->length, fp);
	fwrite("</td></tr>", 1, 10, fp);
	fclose(fp);
	int len = snprintf(body, 999 + 8 + 9 + 10 + username->length + message->length, "<!DOCTYPE html><head><style>table,th,td{border: 1px solid black;}</style></head><body><h1>Form Recieved!</h1><table><tr><td>Username</td><td>Message</td></tr><tr><td>%s</td><td>%s</td></tr></table></body>", username->value, message->value);
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
	static char *body = "<!DOCTYPE html><body><h1>404</h1><p>No such page found!!</p></body>";
	int len = strlen(body);
	static char d[120] = {0};
	sprintf(d, "%d", len);
	appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
	return (response_t){404, "Not Found", headers, body, len+1};
}

#endif
