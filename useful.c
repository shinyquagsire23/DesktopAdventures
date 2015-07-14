#include "useful.h"

#include <stdio.h>

char* readFileToBytes(char* file, long *size)
{
	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(file, "rb");
	fseek(fileptr, 0, SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);

	buffer = (char *) malloc((filelen + 1) * sizeof(char));
	fread(buffer, filelen, 1, fileptr);
	fclose(fileptr);

	*size = filelen;
	return buffer;
}
