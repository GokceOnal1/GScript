#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include "exception.h"

/*Fast way of getting all text from a filepath*/
char* get_file_data(const char* filepath) {
	char* buffer = 0;
	long length;
	FILE* file = fopen(filepath, "rb");
	if(file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = calloc(length, length);

		if(buffer) {
			fread(buffer, 1, length, file);
		}
		fclose(file);
		return buffer;
	}
	_GSERR(0, "GS103 Failed reading of entry input stream");
	exit(2);
}