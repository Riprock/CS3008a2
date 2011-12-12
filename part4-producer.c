/*
  Assessment 2 Part 4 Producer
  Fergal Hainey
*/

#define DEBUG 0
#include "fdebug.h"

#include "part4-common.h"
#include "fcrc.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

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
	char temp[MAX_MSG_LENGTH + 3];
	int mlength;
	uint32_t crc32table[256];
	make_crc32_table(crc32table);
	uint32_t cksum;
	
	int semid = semget(SEMKEY, 2, IPC_CREAT | 0600);
	REPORT_EXIT_ERROR(semid);
	
	int shmid = shmget(SHMKEY, 4096, IPC_CREAT | 0600);
	REPORT_EXIT_ERROR(shmid);
	unsigned char *shm = (unsigned char*) shmat(shmid, 0, 0);
	REPORT_EXIT_ERROR(shm);
	
	/*
	  A brief explanation of the protocol of the shared memory.
	  
	  Semaphores are used for synchronising access. The producer gets semaphore 
	  0 and consume gets 1. When your semaphore value becomes 0 it's time to 
	  work.
	  
	  The first byte is the status. When a process starts working again and the 
	  status is non zero, give up and destroy the shared memory.
	  
	  The next 4 bytes are the length of the message as a signed 32 bit int.
	  
	  The next whatever many bytes are the message.
	  
	  The 4 bytes after the message are the CRC32 checksum of the message. 
	  Remember to leave enough space in the shared memory for this.
	*/
	
	
	struct sembuf sops[1];
	sops[0].sem_flg = 0;
	
	int semdef[] = {1, 0};
	semctl(semid, 0, SETALL, semdef); // The producer works first
	
	FILE *random = fopen("/dev/urandom", "r"); // Random data from /dev/urandom
	/*
	  Long strings can fill the buffer before the line buffer works. Best to 
	  disable buffering to get a nicely synchronised file.
	*/
	FILE *evidence = fopen("part4-evidence.txt", "a");
	setvbuf(evidence, 0, _IONBF, 0);
	
	while (transmitted < CONSUME_TARGET) {
		sops[0].sem_num = 0;
		sops[0].sem_op = -1;
		SHOW_EXIT_ERROR(semop, semid, sops, 1); // Wait my turn.
		DPRINTF("Producer working\n");
		if (shm[0] != 0) break; // It's all gone wrong, skip to closing.
		fread(temp, 1, MAX_MSG_LENGTH + 3, random);
		mlength = ((int) ((*((uint32_t*) &temp[0])) % MAX_MSG_LENGTH) + 1);
		DPRINTF("%d\n", mlength);
		int i;
		for(i = 0; i < mlength; i++) {
			temp[4 + i] = 'a' + (((unsigned char) temp[4 + i]) % 26);
		}
		temp[3 + mlength] = 0; // String terminator
		cksum = crc32(mlength, &temp[4], crc32table);
		memcpy(&shm[1], &mlength, 4); // Length in shared memory
		memcpy(&shm[5], &temp[4], mlength); // Message in shared memory
		memcpy(&shm[5 + mlength], &cksum, 4); // Checksum in shared memory
		
		fwrite(&temp[4], 1, mlength - 1, evidence);
		fwrite("\n", 1, 1, evidence);
		
		shm[0] = 0;
		
		DPRINTF("Producer not working\n");
		sops[0].sem_num = 1;
		sops[0].sem_op = 1;
		SHOW_EXIT_ERROR(semop, semid, sops, 1);
		
		transmitted++;
	}
	
	sops[0].sem_num = 0;
	sops[0].sem_op = -1;
	SHOW_EXIT_ERROR(semop, semid, sops, 1); // Wait my turn.
	fclose(random);
	fclose(evidence);
	shmdt(shm); // Detach shared memory
	shmctl(shmid, IPC_RMID, 0); // Destroy it
	semctl(semid, 0, IPC_RMID); // Destroy semaphores
	DPRINTF("p %d\n", mlength);
}
