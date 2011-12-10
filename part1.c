/*
  CS3008 Assessment 2 Part 1
  Fergal Hainey
*/

#include <stdio.h>

void makefile(const char *filename, int length) {
	FILE *file;
	file = fopen(filename, "w");
	int i;
	for (i = 0; i < length; i++) {
		fputc(0, file);
	}
	fclose(file);
}

int main(int argc, const char **argv) {
	makefile("test0.bin", 0);
	makefile("test1.bin", 1);
	makefile("test40129.bin", 40129);
};
