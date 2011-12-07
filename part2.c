/*
  CS3008 Assessment 2 Part 2
  Fergal Hainey
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE *readpic, *writepic;
	readpic = fopen("testread.gif", "rb");
	fseek(readpic, 0, SEEK_END);
	int size = ftell(readpic);
	rewind(readpic);
	void *mem = malloc(size);
	fread(mem, 1, size, readpic);
	fclose(readpic);
	
	writepic = fopen("testwrite.gif", "w");
	int i;
	fwrite(mem, 1, size, writepic);
	fclose(writepic);
}
