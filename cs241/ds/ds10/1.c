#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void *fun1(void *ptr)
{
	int fd = *((int *)ptr);
	sleep(2);

	char *buffer = "Hello!";
	write(fd, buffer, strlen(buffer));
	close(fd);

	return NULL;
}

void *fun2(void *ptr)
{
	int fd = *((int *)ptr);

	char *buffer = "Hi!";
	write(fd, buffer, strlen(buffer));
	close(fd);

	return NULL;
}


void main()
{
  int pipes[2], pipes1[2];
  
  pipe(pipes);
  pipe(pipes1);

  if(!fork()) {
    close(pipes[0]);
    fun((void*)pipes[1]);
    exit(0);
  }

  if(!fork()) {
    close(pipes1[0]);
    close(pipes[0]);
    close(pipes[1]);
    fun2((void*)pipes1[1]);
    exit(0);
  }

  close(pipes[1]);
  close(pipes1[1]);

  /* FD_SET(pipes[1], ...); */
}
