/*
  Assessment 2 Part 3 Consumer
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
	uint32_t newcksum;
	
	int shmid = shmget(shmkey, 4096, 0600);
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
	unsigned char *shm = (unsigned char*)shmat(shmid, 0, 0);
	if (shm < 0) return 2;
	
	while (transmitted < target) {
		while ((char) shm[0] != 'c');
		if (transmitted == 0) printf("Consumer working\n");
		memcpy(&mlength, &shm[2], 4);
		memcpy(temp, &shm[6], mlength);
		memcpy(&cksum, &shm[6 + mlength], 4);
		newcksum = crc32(mlength, temp, crc32table);
		// Uncomment the next line to test crisis situations!
		//if (transmitted > 5000000) newcksum = 0xFFFFFFFF;
		if (cksum != newcksum) {
			printf(
				"I have failed! %08x != %08x after %d/%d bytes transmitted.\n",
				newcksum, cksum, transmitted, target
			);
			shm[1] = 1;
			shm[0] = (unsigned char) 'p';
			break;
		}
		shm[1] = 0;
		shm[0] = (unsigned char) 'p';
		
		transmitted += mlength;
	}
	
	shmdt(shm);
	if (transmitted == target)
		printf("Success! Cheksums match for each transmission.\n");
}
