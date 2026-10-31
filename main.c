#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>

#include "consts.h"
#include "request.h"
#include "response.h"
#include "endpoints.h"

#ifdef DEBUG
int requestCount = 0;
#endif

int main(int argc, char **argv){
	loadEndpoints; // macro
	int socketOpt = 1, serverFd;
	if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr,"Failed opening a socket\n");
		return 1;
	}
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socketOpt, sizeof(socketOpt))){

		fprintf(stderr,"Failed attaching socket\n");
		return 1;
	}
	struct sockaddr_in socketAddress;
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = INADDR_ANY;
        socketAddress.sin_port = htons(PORT);
        if(bind(serverFd, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) < 0){
                fprintf(stderr,"Failed binding port to socket\n");
		return 1;
        }
        if(listen(serverFd, 3) < 0){
                fprintf(stderr,"Failed binding port to socket\n");
                return 1;
        }
        int newRequest;
        socklen_t addrlen = sizeof(socketAddress);
        printf("Server started on port %d\n", PORT);
        while(1){
		int pid,status;
		while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
#ifdef DEBUG
			printf("Terminated child process %d\n", pid);
#endif
		}
                if ((newRequest = accept(serverFd, (struct sockaddr*)&socketAddress, &addrlen)) < 0) {
                        fprintf(stderr,"Failed getting new request\n");
                        continue;
                }
                int forkResult = fork();
                if(forkResult == -1){
                        fprintf(stderr,"Failed making child process for request\n");
                        continue;
                } else if(forkResult){
                        close(newRequest);
#ifdef DEBUG
			requestCount += 1;
#endif
                        continue;
                }
		close(serverFd); // we're inside a fork
#ifdef DEBUG
		struct timeval start, end;
		gettimeofday(&start, NULL);
#endif
		char *predata = malloc(PREDATASIZE); // basically data to find the HTTP header with
		predata[0] = 0;
		char *predatastart = predata;
		long datafillsize = read(newRequest, predata, PREDATASIZE-1);
#ifdef DEBUG
		requestCount += 1;
		printf("Request ID: #%d\nPredata size: %ld\nPredata used: %ld\n", requestCount, (long)PREDATASIZE, datafillsize);
		if(datafillsize >= PREDATASIZE-1)
			printf("Second read() will be required!\n");
		//printf("Data:\n%s\n", predata);
#endif
		// start parsing request
		// get the path
		char *type, *path, *version;
		int typeLength, pathLength, versionLength;
		typeLength = pathLength = versionLength = 0;
type:
		
		type = predata;
		for(;;predata++){
			if(*predata == ' '){
				*predata = 0;
				predata++;
				goto path;
			} else if(*predata > 'Z' || *predata < 'A'){
				fprintf(stderr, "Encountered malformed type\n");
				goto forceclose;
			}
			typeLength++;
		}
path:
		path = predata;
		for(;;predata++){
			if(*predata == ' '){
				*predata = 0;
				predata++;
				goto version;
			} else if(*predata == '\n' || *predata == '\0' || *predata == '\r'){ //hmmm, suspicious...
				fprintf(stderr, "Encountered malformed path\n");
				goto forceclose;
			}
			pathLength++;
		}
version:
		version = predata;
		for(;;predata++){
			if(*predata == '\n'){
				*predata = 0;
				predata++;
				goto headerslabel;
			} else if(*predata == '\r'){
				*predata = 0;
				continue;
			} else if(*predata == '\0' || *predata == ' '){
				fprintf(stderr, "Encountered malformed version\n");
				goto forceclose;
			}
			versionLength++;
		}
headerslabel:
		if(strcmp(type,"POST") && strcmp(type,"GET")){
			fprintf(stderr, "Recieved neither a GET nor POST request, unsupported!");
			goto forceclose;
		}
		char *headers = predata;
		char *currentHeader = predata;
		int headerLength = 0;
		header_t *parsedHeaders = newHeaders();
#ifdef DEBUG
		printf("Type[%d]: %s\nPath[%d]: %s\nVersion[%d]: %s\n", typeLength, type, pathLength, path, versionLength, version);
#endif
		for(;;predata++){
			if(*predata == '\n'){
				*predata = 0;
#ifdef DEBUG
				printf("%lx:\n\t%s\n", (long) currentHeader, currentHeader);
#endif
				appendHeader(parsedHeaders, parseHeader(currentHeader)); // might make this function inline in this loop since looking backwards
											 // in a string already iterated through seems silly
				currentHeader = predata+1;
			} else if(*predata == '\r'){
				*predata = 0;
				if(predata == currentHeader){
					predata += 2; // this is a hack and should be done properly
					goto body;
				}
			} else if(*predata == '\0'){
				fprintf(stderr, "Encountered malformed header\n");
				goto forceclose;
			}
			headerLength++;
		}
body:
#ifdef DEBUG
		printf("Parsed headers:\n");
		for(int i = 0; i < parsedHeaders->count; i++){
			printf("\t%s : %s\n", parsedHeaders->headers[i].key, parsedHeaders->headers[i].value);
		}
		printf("Started parsing body\n");
#endif
		char *contentlengthc = searchHeader(parsedHeaders, "Content-Length");
		long contentlength = -1;
		char *bodystart = NULL;
		if(contentlengthc){
			contentlength = atol(contentlengthc);
			if(contentlength > PREDATASIZE-(predata-predatastart)){
				char *temp = malloc(contentlength);
				memcpy(temp, predata, PREDATASIZE-(predata-predatastart)-1);
				read(newRequest, temp+PREDATASIZE-(predata-predatastart)-1, contentlength-(PREDATASIZE-(predata-predatastart)-1));
				predata = temp;
				// cannot free predata/predatastart here because many strings depend on them
			}
#ifdef DEBUG
			printf("Content-Length is %ld\n", contentlength);
#endif
		}
		char *body = predata;
		long bodyLength = contentlength;
#ifdef DEBUG
		printf("Body data:\n%s\n",body);
		printf("Expected size: %ld, strlen size: %ld\n", bodyLength, strlen(body));
#endif
		response_t (*func)(request_t) = getEndpoint(path);
		if(func){
#ifdef DEBUG
			printf("Found function at %lx\n", (long unsigned int)func);
#endif
			response_t response = func((request_t){type, path, parsedHeaders, body});
			char *finalResponse = malloc(response.responseSize);
			char *compiledHeaders = malloc(response.responseSize);
			for(int i = 0; i<response.headers->size; i++){
				strcat(compiledHeaders, response.headers->headers[i].key);
				strcat(compiledHeaders, ": ");
				strcat(compiledHeaders, response.headers->headers[i].value);
				strcat(compiledHeaders, "\r\n");
			}
			snprintf(finalResponse, response.responseSize, "HTTP/1.1 %d %s\r\n%s\r\n%s", response.statusCode, response.statusText, compiledHeaders, response.body);
#ifdef DEBUG
			printf("Responding:\n%s\n", finalResponse);
#endif
			send(newRequest, finalResponse, response.responseSize, 0);
		} else {
#ifdef DEBUG
			printf("Should send 404 page\n");
#endif
		}
		free(parsedHeaders->headers);
		free(parsedHeaders);
		if(bodystart)
			free(bodystart);
forceclose:
		close(newRequest);
		free(predatastart);
#ifdef DEBUG
		printf("Closing fork\n");
		gettimeofday(&end, NULL);
		printf("Took %ld microseconds\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
#endif
		return 0;
	}
}
