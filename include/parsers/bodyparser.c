#ifndef BODYPARSER_C
#define BODYPARSER_C

#include "bodyparser.h"
#include "request.h"
#include "headers.h"
#include "response.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int isLineBoundary(char *line, char *boundary){
	if(line[0] != '-' || line[1] != '-'){
		return 0;
	}
	line = line+2;
	for(;*boundary; boundary++)
		if(*line++ != *boundary)
			return 0;
	if(line[0] == '-' && line[1] == '-')
		return 2;
	return 1;
} // \r\n terminated line

parsedBody_t parseMultiFormData(request_t request){
	char *headerIter = searchHeader(request.headers, "Content-Type");
	char *contentType = headerIter;
	if(!contentType)
		return (parsedBody_t){-1, -1, NULL};	
	headerIter += 19; // length of multipart/form-data + 1
	if(*headerIter != ';')
		return (parsedBody_t){-1, -1, NULL};	
	char temp = *headerIter; // so we can recover the header
	*headerIter = 0; // make sure you know whether doing this is safe
	if(strcmp(contentType, "multipart/form-data")){
		*headerIter = temp;
		return (parsedBody_t){-1, -1, NULL};	
	}
	*headerIter = temp;
	char *boundary = NULL;
	// iterate through to find boundary
	for(;*headerIter;headerIter++)
		if(*headerIter == '='){ // assume this defines the boundary
			boundary = headerIter + 1;
			break;
		}
	if(!boundary)
		return (parsedBody_t){-1, -1, NULL};	
	char *bodyIter = request.body;
	int result;
	char *currentValue = NULL;
	char *currentParams = NULL;
	parsedBody_t parsedBody = {2, 0, malloc(sizeof(parsedData_t)*2)};
	for(;*bodyIter;bodyIter++){
		result = isLineBoundary(bodyIter, boundary);
		switch(result){
			case 1:
				if(currentValue)
					*(bodyIter-2) = 0;
				if(currentParams)
					goto readdata;
backin:
				for(;*bodyIter != '\n'; bodyIter++);
				bodyIter++;
				currentParams = bodyIter;
				for(;*bodyIter != '\r'; bodyIter++);
				*bodyIter = 0;
				bodyIter = bodyIter + 4;
				currentValue = bodyIter;
				break;
			case 2:
				*(bodyIter-2) = 0;
				if(currentParams)
					goto readdata;
				break;
			default:
				break;
		}
		for(;*bodyIter != '\n'; bodyIter++);
	}
readdata:
	if(parsedBody.size == parsedBody.count){
		parsedBody.size *= 2;
		parsedBody.data = realloc(parsedBody.data, sizeof(parsedData_t) * parsedBody.size);
	}
	parsedBody.data[parsedBody.count].size = 2;
	parsedBody.data[parsedBody.count].count = 0;
	parsedBody.data[parsedBody.count].params = malloc(sizeof(keyvalue_t) * 2);
	parsedBody.data[parsedBody.count].value = currentValue;
	char *currentField = currentParams;
	char *currentFieldValue = NULL;
	for(;;currentParams++){
		if(*currentParams == ':' || *currentParams == '='){
			*currentParams = 0;
			currentParams++;
			for(;*currentParams == ' '; currentParams++);
			currentFieldValue = currentParams;
		} else if(*currentParams == ';'){
			*currentParams = 0;
			currentParams++;
			if(parsedBody.data[parsedBody.count].count == parsedBody.data[parsedBody.count].size){
				parsedBody.data[parsedBody.count].size *= 2;
				parsedBody.data = realloc(parsedBody.data[parsedBody.count].params, sizeof(keyvalue_t) * parsedBody.data[parsedBody.count].size);
			}
			parsedBody.data[parsedBody.count].params[parsedBody.data[parsedBody.count].count].key = currentField;
			parsedBody.data[parsedBody.count].params[parsedBody.data[parsedBody.count].count].value = currentFieldValue;
			parsedBody.data[parsedBody.count].count++;
			for(;*currentParams == ' '; currentParams++);
			currentField = currentParams;
			currentFieldValue = NULL;
		} else if(*currentParams == '\0'){
			if(parsedBody.data[parsedBody.count].count >= parsedBody.data[parsedBody.count].size){
				parsedBody.data[parsedBody.count].size *= 2;
				parsedBody.data[parsedBody.count].params = realloc(parsedBody.data[parsedBody.count].params, sizeof(keyvalue_t) * parsedBody.data[parsedBody.count].size);
			}
			parsedBody.data[parsedBody.count].params[parsedBody.data[parsedBody.count].count].key = currentField;
			parsedBody.data[parsedBody.count].params[parsedBody.data[parsedBody.count].count].value = currentFieldValue;
			parsedBody.data[parsedBody.count].count++;
			break;
		}
	}
	parsedBody.count++;
	if(result == 2)
		goto out;
	goto backin;
out:
#ifdef DEBUG
	for(int i = 0; i < parsedBody.count; i++){
		printf("Value: %s\nAdditional Params[%d]:\n", parsedBody.data[i].value, parsedBody.data[i].count);
		for(int j = 0; j < parsedBody.data[i].count; j++){
			printf("\tKey: %s | Value %s\n", parsedBody.data[i].params[j].key, parsedBody.data[i].params[j].value);
		}
	}
#endif
	return parsedBody;
}

#endif
