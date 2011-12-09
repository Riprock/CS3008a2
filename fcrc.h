/*
  Fergal's CRC stuff
  
  Should be compatible with the Ethernet spec CRC32 (mostly 'cos I wanted 
  something easily available to check my results against, namely `cksum -o3`).
*/

#include <stdint.h>

void make_crc32_table(uint32_t *table);

uint32_t crc32(int length, char *message, uint32_t *table);
