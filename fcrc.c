/*
  Fergal's CRC stuff
  
  A quicker table based algorithm. I figured if we're working on 10,000,000 
  bytes may as well make it fast.
*/

#include "fcrc.h"

uint32_t polynomial = 0x04C11DB7;
uint32_t initial_remainder = 0xFFFFFFFF;
uint32_t final_xor = 0xFFFFFFFF;

uint32_t reflect(uint32_t subject, int bits) {
	uint32_t reflection = 0;
	int b;
	for (b = 0; b < bits; b++) {
		if ((subject >> b) & 1)
		{
			reflection |= (1 << (bits - 1 - b));
		}
	}
	
	return reflection;
}

void make_crc32_table(uint32_t *table) {
	uint32_t remainder;
	uint32_t dividend;
	int bit;
	
	for (dividend = 0; dividend < 256; dividend++) {
		remainder = (dividend << 24);
		for (bit = 8; bit > 0; bit--) {
			if (remainder & (1 << 31))
				remainder = ((remainder << 1) ^ polynomial);
			else remainder = (remainder << 1);
		}
		
		table[dividend] = remainder;
	}
}

uint32_t crc32(int length, char *message, uint32_t *table) {
	uint32_t remainder = initial_remainder;
	unsigned char byte;
	
	int i;
	for (i = 0; i < length; i++) {
		byte = (reflect(message[i], 8) ^ (remainder >> 24));
		remainder = (table[byte] ^ (remainder << 8));
	}
	
	return (reflect(remainder, 32) ^ final_xor);
}
