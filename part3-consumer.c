/*
  Assessment 2 Part 3 Consumer
  Fergal Hainey
*/

#define DEBUG 0
#include "fdebug.h"

#include "part3-common.h"
#include "fcrc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main() {
	int transmitted  = 0;
	char temp[MAX_MSG_LENGTH];
	int mlength;
	uint32_t crc32table[256];
	make_crc32_table(crc32table);
	uint32_t cksum;
	uint32_t newcksum;
	uint32_t pastcksum;
	
	int semid = semget(SEMKEY, 2, 0600);
	REPORT_EXIT_ERROR(semid);
	
	int shmid = shmget(SHMKEY, 4096, 0600);
	REPORT_EXIT_ERROR(shmid);
	unsigned char *shm = (unsigned char*)shmat(shmid, 0, 0);
	REPORT_EXIT_ERROR(shm);
	
	struct sembuf sops[1];
	sops[0].sem_flg = 0;
	
	while (transmitted < CONSUME_TARGET) {
		sops[0].sem_num = 1;
		sops[0].sem_op = -1;
		SHOW_EXIT_ERROR(semop, semid, sops, 1); // Wait my turn.
		DPRINTF("Consumer working\n");
		memcpy(&mlength, &shm[1], 4);
		memcpy(temp, &shm[5], mlength);
		memcpy(&cksum, &shm[5 + mlength], 4);
		newcksum = crc32(mlength, temp, crc32table);
		// Uncomment the next line to test crisis situations!
		//if (transmitted > 5000000) newcksum = 0xFFFFFFFF;
		if (cksum != newcksum) {
			printf(
				"I have failed! %08x != %08x after %d/%d bytes transmitted.\n",
				newcksum, cksum, transmitted, CONSUME_TARGET
			);
			shm[0] = 1;
		}
		if (newcksum == pastcksum) {
			printf(
				"Duplicate checksum %08x after %d/%d bytes transmitted.\n"
				"The shared memory is being crap.\n",
				newcksum, cksum, transmitted, CONSUME_TARGET
			);
			shm[0] = 1;
		}
		
		DPRINTF("Consumer not working\n");
		sops[0].sem_num = 0;
		sops[0].sem_op = 1;
		SHOW_EXIT_ERROR(semop, semid, sops, 1);
		if (cksum != newcksum || newcksum == pastcksum) break;
		
		pastcksum == newcksum;
		
		transmitted += mlength;
	}
	
	shmdt(shm);
	DPRINTF("c %d\n", mlength);
	if (transmitted == CONSUME_TARGET)
		printf("Success! Checksums match for each transmission.\n");
	else DPRINTF("c %d\n", transmitted);
}
