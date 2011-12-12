/*
  Assessment 2 Part 5
  Fergal Hainey
*/

#define MAPPED_SIZE 4096

#define DEBUG 0
#include "fdebug.h"

#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

struct Payload {
	char *string;
};
struct List {
	struct List *next;
	struct Payload *payload;
};

int main(int argc, char **argv) {
	FILE *file = fopen("part5-mapped.bin", "a+");
	rewind(file);
	if (fgetc(file) == EOF) {
		DPRINTF("Making file\n");
		int i;
		for (i = 0; i < MAPPED_SIZE; i++) {
			fputc(0, file);
		}
		fflush(file);
	}
	fclose(file);
	int f = open("part5-mapped.bin", O_RDWR, 0);
	unsigned char *mapped = (unsigned char*) mmap(
		0,
		MAPPED_SIZE,
		PROT_WRITE | PROT_READ,
		MAP_SHARED,
		f,
		0
	);
	
	// -r just means read back. Otherwise map a list of strings.
	if (argc < 2 || (argc > 1 && strncmp(argv[1], "-r", 2) != 0)) {
		int bytes_used = 0;
		int i;
		for (i = 0; i < 20; i++) {
			char string[25];
			sprintf(string, "This is string number %d", i);
			int string_length = strlen(string);
			/*
			  Pointers start from 0, the beginning of the file. This is so when 
			  the file is mapped again, they can be adapted to the new address.
			*/
			struct Payload payload = { 
				(char*) (
					bytes_used
					+ sizeof(struct List)
					+ sizeof(struct Payload)
				)
			};
			struct List list = {
				(struct List*) (
					(i < 19)
					? (
						bytes_used
						+ sizeof(struct List)
						+ sizeof(struct Payload)
						+ string_length
						+ 1
					)
					: 0
				),
				(struct Payload*) (
					bytes_used
					+ sizeof(struct List)
				)
				
			};
			
			DPRINTF(
				"%p %p %d\n", &mapped[bytes_used], &list, sizeof(struct List));
			memcpy(&mapped[bytes_used], &list, sizeof(struct List));
			bytes_used += sizeof(struct List);
			memcpy(&mapped[bytes_used], &payload, sizeof(struct Payload));
			bytes_used += sizeof(struct Payload);
			memcpy(&mapped[bytes_used], string, string_length + 1);
			bytes_used += string_length + 1;
		}
	}
	else if (argc > 1 && strncmp(argv[1], "-r", 2) == 0) {
		// Walk the list and print the strings.
		struct List *rlist = (struct List*) &mapped[0];
		while (rlist) {
			DPRINTF("%p %x\n", rlist->payload, ((long) rlist->payload));
			struct Payload *payload =
				(struct Payload*) &mapped[(long) rlist->payload];
			char *string = (char*) &mapped[(long) payload->string];
			printf("%s\n", string);
			rlist = (long) rlist->next != 0
			        ? (struct List*) &mapped[(long) rlist->next]
			        : 0;
		}
	}
	munmap(mapped, MAPPED_SIZE);
	close(f);
}
