#ifndef UTILS_C
#define UTILS_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "bodyparser.h"

#define expandNew() if(count>=size-10){size*=2; new = realloc(new, size);}

void HTMLEscape(parsedData_t *data){ // replace \r\n with <br>, replace <,>,&,",'
	int count = data->length;
	int size = (count+1)*2;
	char *new = malloc(size);
	memcpy(new, data->value, count+1);
	for(int i = 0; new[i]; i++){
		switch(new[i]){
			case '\r':
				if(new[i+1] == '\n'){
					count += 2;
					expandNew();
					memmove(new+2+i, new+i, size-i-2);
					new[i++] = '<';
					new[i++] = 'b';
					new[i++] = 'r';
					new[i] = '>';
				}
				break;
			case '<':
				count += 3;
				expandNew();
				memmove(new+3+i, new+i, size-i-3);
				new[i++] = '&';
				new[i++] = 'l';
				new[i++] = 't';
				new[i] = ';';
				break;
			case '>':
				count += 3;
				expandNew();
				memmove(new+3+i, new+i, size-i-3);
				new[i++] = '&';
				new[i++] = 'g';
				new[i++] = 't';
				new[i] = ';';
				break;
			case '&':
				count += 4;
				expandNew();
				memmove(new+4+i, new+i, size-i-4);
				new[i++] = '&';
				new[i++] = 'a';
				new[i++] = 'm';
				new[i++] = 'p';
				new[i] = ';';
				break;
			case '"':
				count += 5;
				expandNew();
				memmove(new+5+i, new+i, size-i-5);
				new[i++] = '&';
				new[i++] = 'q';
				new[i++] = 'u';
				new[i++] = 'o';
				new[i++] = 't';
				new[i] = ';';
				break;
			case '\'':
				count += 4;
				expandNew();
				memmove(new+4+i, new+i, size-i-4);
				new[i++] = '&';
				new[i++] = '#';
				new[i++] = '3';
				new[i++] = '9';
				new[i] = ';';
				break;
		}
	}
	data->value = new;
	data->length = count;
}

#endif
