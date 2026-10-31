#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <time.h>

#include "consts.h"
#include "body.h"
#include "response.h"

#ifdef DEBUG
int requestCount = 0;
#endif

// where our endpoint functions will be stored
response_t *(*endpointHashMap[ENDPOINTHASHSIZE])(char *, body_t *) = {NULL, };

int main(int argc, char **argv){
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
		char *predata = calloc(PREDATASIZE, sizeof(char)); // basically data to find the HTTP header with
		char *predatastart = predata;
		long datafillsize = read(newRequest, predata, PREDATASIZE-1);
#ifdef DEBUG
		requestCount += 1;
		printf("Request ID: #%d\nPredata size: %ld\nPredata used: %ld\n", requestCount, (long)PREDATASIZE, datafillsize);
		if(datafillsize >= PREDATASIZE-1)
			printf("Second read() will be required!\n");
		printf("Data:\n%s\n", predata);
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
				goto headers;
			} else if(*predata == '\r'){
				*predata = 0;
				continue;
			}else if(*predata == '\0' || *predata == ' '){
				fprintf(stderr, "Encountered malformed version\n");
				goto forceclose;
			}
			versionLength++;
		}
headers:
#ifdef DEBUG
		printf("Type[%d]: %s\nPath[%d]: %s\nVersion[%d]: %s\n", typeLength, type, pathLength, path, versionLength, version);
#endif
forceclose:
		close(newRequest);
		free(predatastart);
		return 0;
	}
}
