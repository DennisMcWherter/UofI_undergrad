/**
 * mmap-example.c
 *
 * Example
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  size_t written = 0;
  off_t last = 0;
  int fd, pageSize = 0;
  const char* text = "Testing!";
  const char* text2 = "I am process two!!!!!\n";
  void* addr;
  int len;
  off_t myoff = 0;
  len = pageSize;

  if((pageSize = sysconf(_SC_PAGE_SIZE)) < 0) {
    return -1;
  }

  len = pageSize;

  fd = open("/tmp/w00t", (O_CREAT | O_TRUNC | O_RDWR), S_IRWXU | S_IRWXG);

  if(fd < 0)
    return -1;

  last = lseek(fd, pageSize, SEEK_SET);
  written = write(fd, "x", 1);

  if(written != 1)
    return -1;

  addr = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, myoff);

  if(addr == MAP_FAILED) {
    fprintf(stderr, "Fail!\n");
    return -1;
  }

  strcpy((char*)addr, text);

  msync(addr, pageSize, MS_SYNC);

  if(!fork()) {
    strcpy((char*)addr, text2);
    msync(addr, pageSize, MS_SYNC);
  }

  close(fd);
  unlink("/tmp/w00t");

  return 0;
}

