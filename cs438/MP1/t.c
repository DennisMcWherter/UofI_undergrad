#include <stdio.h>
#include <stdlib.h>


int main()
{
  int byte = 0;
  int shift = 0;
  int next = 0;
  int total = 0;
  int beef = 0xefbeadde;
  int mask = 0xff;
  char* p = (char*)&beef;
  int t = p[0] & 0xff;
  t = (t << 8) | (p[1] & 0xff);
  printf("0x%x, 0x%x, 0x%x, 0x%x, %d, %d\n", beef, mask,
   t ^ 0x0fff,
    p[0] & 0xff,
    (beef & (mask << 1) != 0) ? 1 : 0,
    (beef & (mask << 2) != 0) ? 1 : 0);
  int i = 0;
  int j = 0;
  int msk  = 0x800;
  int poly = 0x80d;
  printf("mask: ");
  for(j = 0 ; j < 32 ; ++j) {
    byte = j / 8;
    shift = j % 8;
    next = ((0x80 >> shift) & ((char*)(&msk))[byte]) >> (7 - shift);
    printf("%d", next);
  }
  printf("\n\n");
  printf("poly: ");
  for(j = 0 ; j < 32 ; ++j) {
    byte = j / 8;
    shift = j % 8;
    next = ((0x80 >> shift) & ((char*)(&poly))[byte]) >> (7 - shift);
    printf("%d", next);
  }
  printf("\n\n");
  for(i = 0 ; i < 32 ; ++i) {
    byte = i / 8;
    shift = i % 8;
    next = (((0x80) >> shift) & ((char*)&beef)[byte]) >> (7 - shift);
    total = (total << 1) | next;
    printf("total (%2d): ", i+1);
  for(j = 0 ; j < 32 ; ++j) {
    byte = j / 8;
    shift = j % 8;
    next = ((0x80 >> shift) & ((char*)(&total))[byte]) >> (7 - shift);
    printf("%d", next);
  }
  printf("\n");
  }


  int test = 0x1234;
  int rem = 0;

  for(i = 0 ; i < 32 ; ++i) {
    byte = i / 8;
    shift = i % 8;
    next = ((0x80 >> shift) & ((char*)(&test))[byte]) >> (7 - shift);
    rem = (rem << 1) | next;
    if(0x800 & rem) {
      rem ^= poly;
    }
  }

  for(i = 0 ; i < 12 ; ++i) {
    if(rem == 0)
      break;
    rem <<= 1;
    rem |= 0;
    if(0x800 & rem) {
      rem ^= poly;
    }
  }

  printf("rem: 0x%x\n", rem);

  return 0;
}

