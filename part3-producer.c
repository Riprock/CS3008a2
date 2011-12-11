/*
  Assessment 2 Part 3 Producer
  Fergal Hainey
*/

#include "fcrc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void byte2string(unsigned char subject, char *dst) {
	int i;
	for (i = 0; i < 8; i++) {
		if ((subject >> i) & 1) dst[i] = '1';
		else dst[i] = '0';
	}
	dst[8] = 0;
}

void printbytes(int length, unsigned char *bytes) {
	int i;
	char binary[9];
	for (i = 0; i < length; i++) {
		byte2string(bytes[i], binary);
		printf("%s ", binary);
	}
	printf("\n");
}

int main() {
	int transmitted  = 0;
	int target = 10000000;
	int maxmlength = 4086;
	key_t shmkey = 18021;
	char temp[maxmlength];
	int mlength;
	uint32_t crc32table[256];
	make_crc32_table(crc32table);
	uint32_t cksum;
	
	int shmid = shmget(shmkey, 4096, IPC_CREAT | 0600);
	if (shmid < 0) {
		switch (errno) {
			case EACCES:
				printf("EACCES\n");
				break;
			case EEXIST:
				printf("EEXIST\n");
				break;
			case EINVAL:
				printf("EINVAL\n");
				break;
			case ENOENT:
				printf("ENOENT\n");
				break;
			case ENOMEM:
				printf("ENOMEM\n");
				break;
			case ENOSPC:
				printf("ENOSPC\n");
				break;
		}
		return 1;
	}
	unsigned char *shm = (unsigned char*) shmat(shmid, 0, 0);
	if (shm < 0) return 2;
	
	/*
	  A brief explanation of the protocol of the shared memory.
	  
	  The first byte is who we are waiting for: 'p' for producer, 'c' for 
	  consumer. If this byte changes to the one the process is supposed to be 
	  listening for, the process gets to work.
	  
	  The next byte is the status. When a process starts working again and the 
	  status is non zero, give up and destroy the shared memory.
	  
	  The next 4 bytes are the length of the message as a signed 32 bit int.
	  
	  The next whatever many bytes are the message.
	  
	  The 4 bytes after the message are the CRC32 checksum of the message. 
	  Remember to leave enough space in the shared memory for this.
	*/
	
	shm[0] = (unsigned char) 'p'; // We're currently waiting for this process
	
	FILE *random = fopen("/dev/urandom", "r"); // Random data from /dev/urandom
	
	while (transmitted < target) {
		while ((char) shm[0] != 'p'); // Wait my turn.
		if (transmitted == 0) printf("Producer working\n");
		if (shm[1] != 0) break; // It's all gone wrong, skip to closing.
		if ((target - transmitted) > maxmlength) mlength = maxmlength;
		else mlength = target - transmitted;
		fread(temp, 1, mlength, random);
		cksum = crc32(mlength, temp, crc32table);
		memcpy(&shm[2], &mlength, 4); // Length in shared memory
		memcpy(&shm[6], temp, mlength); // Message in shared memory
		memcpy(&shm[6 + mlength], &cksum, 4); // Checksum in shared memory
		shm[1] = 0;
		shm[0] = (unsigned char) 'c';
		
		transmitted += mlength;
	}
	
	shmdt(shm); // Detach shared memory
	shmctl(shmid, IPC_RMID, 0); // Destroy it
	fclose(random);
}
