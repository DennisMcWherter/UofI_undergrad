/**
 * signal.c
 * 
 * Example
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void test(int sig) {
  printf("x\n");
}

int main()
{
  struct sigaction myset;
  int sig = 0;

  sigemptyset(&myset.sa_mask);

  myset.sa_handler = test;
  sigaddset(&myset.sa_mask, SIGALRM);
  sigaction(SIGALRM, &myset, NULL);

  alarm(3);
  sigwait(&myset.sa_mask, &sig);

  if(sig == SIGALRM)
    printf("signal caught!\n");

  return 0;
}

