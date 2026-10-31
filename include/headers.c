#ifndef HEADERS_C
#define HEADERS_C

#include <stdlib.h>
#include <stdio.h>

#include "headers.h"
#include "consts.h"

int keyHash_(char *data){
	int hash = 0;
	for(char* i = data; *i; i++){
		if((*i<'N' && *i>='A') || (*i<'n' && *i>='a'))
			hash = (hash >> 1) | (hash << sizeof(int)-1);
		else if((*i>='N' && *i<='Z') || (*i>='n' && *i<='z'))
			hash = (hash << 1) | (hash >> sizeof(int)-1);
		hash += (*i^0x9a)*23;
	}
	return hash;
}

header_t *newHeaders(){
	header_t *newHeader = calloc(1, sizeof(header_t));
	newHeader->size = 2;
	newHeader->count = 0;
	newHeader->headers = calloc(newHeader->size, sizeof(keyvalue_t));
	return newHeader;
}

keyvalue_t parseHeader(char *header){
	char *key = header;
	int hash = 0;
	for(;;header++){
		if(*header == ':'){
			*header = 0;
			header++;
			hash = keyHash_(key);
			goto value;
		} else if(*header == '\0'){
			return (keyvalue_t){keyHash_(key),key,NULL};
		}
	}
value:
	char *value = header;
	// maybe do something more
	for(;*value==' ';*value++);
	return (keyvalue_t){hash, key, value};
}

void *appendHeader(header_t *headers, keyvalue_t header){
	if(headers->count >= headers->size){
		headers->size *= 2;
		headers->headers = realloc(headers->headers, sizeof(keyvalue_t)*headers->size);
#ifdef DEBUG
		printf("New Headers (%lx) size: %d\n", (long) headers, headers->size);
#endif
	}
#ifdef DEBUG
	printf("Appending header:\n\tKey - %s\n\tValue - %s\n\tHash - %d\n", header.key, header.value, header.keyHash);
#endif
	headers->headers[headers->count++] = header;
}

char *searchHeader(header_t *headers, char *searchKey){
	int searchHash = keyHash_(searchKey);
	for(int i = 0; i < headers->count; i++)
		if(searchHash == headers->headers[i].keyHash)
			return headers->headers[i].value;
	return NULL;
}

#endif
