/**
 * test.c
 *
 * test crc
 */

#include <stdio.h>
#include <netinet/in.h>

#define BITS_PER_BYTE 8

/**
 * Extract a bit from data (from MSB to LSB)
 *
 *
 * NOTE: This method assumes the data is sufficiently
 *  large to contain the requested bit.
 *
 * @param bit   The bit to extract
 *
 * @return  The bit-value at the specified index
 */
unsigned extractBit(int bit, const void* input)
{
  const char* msg = (const char*)input;
  unsigned byte  = bit / BITS_PER_BYTE;
  unsigned shift = bit % BITS_PER_BYTE;
  char mask = (0x80 >> shift);
  return ((msg[byte] & mask) != 0) ? 1 : 0;
}

void printBit(int num, int bits)
{
  int i = 0;
  for( i = 0 ; i < bits ; ++i)
    printf("%d", extractBit(i, &num));
}

unsigned crc12(short code, const char* msg, size_t bits)
{
  if(msg == NULL)
    return -1;

  const short crc = ntohs(0x80d);
  printf("crc = ");
  printBit(crc, 16);
  printf("\n");
  int i = 0;
  int j = 0;
  unsigned rem = 0;

printf("Inpt: ");
printBit(*(short*)msg, 16);
printf("\n");
printf("Mask: ");
printBit(htons(0x800), 16);
printf("\n\n");

  for( i = 0 ; i < bits ; ++i ) {
  //  rem = ntohs(rem);
    rem = (rem << 1) | extractBit(i, msg);
    //rem = htons(rem);
    printf("Rem = ", rem);
    printBit(rem, 16);
    printf("\n");
    short res = rem & 0x800;//htons(0x800);
    if(res) {
      rem ^= crc;
      printf("xor res = ");
      printBit(rem, 16);
      printf("\n");
    }
  }

  return rem;
}

unsigned crc(const unsigned poly, const unsigned mask, const char* buffer, unsigned length)
{
  unsigned crc = 0;
  int i = 0;
  int j = 0;

  for(i = 0 ; i < length ; ++i) {
    crc ^= (*buffer++ << 4);
    for(j = 0 ; j < 8 ; ++j) {
      crc = (crc & mask) ? (crc << 1) ^ poly : (crc << 1);
      crc &= 0xfff;
    }
  }

  return crc;
}

int main()
{
  short msg = ntohs(0xb0d2);
  printf("0x%x\n", msg);
  short msgr = 0xb0d2;//ntohs(0x80f);//ntohs(msg);
  int i = 0;
//  for(i = 0 ; i < 8*sizeof(msg) ; ++i)
  //  printf("%d", extractBit(i, (void*)&msg));
  //printf("\n");
  //printf("%d\n", crc12(0, msg, sizeof(msg) * 8));
  printf("Int = ");
  for(i = 0 ; i < 8*sizeof(msg) ; ++i)
    printf("%d", extractBit(i, &msg));
  printf("\n");
  for( i = 0 ; i < 8*sizeof(msg); ++i)
    printf("%d", extractBit(i, &msgr));
  printf("\n");
  printf("\n");
  printf("%d\n", crc12(0, (char*)&msgr, sizeof(msgr) * 8));
  printf("\n");


  printf("rem = 0x%x\n", crc(0x180d, 0x800, (char*)&msg, sizeof(msg)));

  return 0;
}

