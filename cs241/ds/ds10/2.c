#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
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
	int fds1[2], fds2[2];
  fd_set set;

  FD_ZERO(&set);

	pipe(fds1);
	int fds1_read = fds1[0];
	int fds1_write = fds1[1];

	pipe(fds2);
	int fds2_read = fds2[0];
	int fds2_write = fds2[1];

	pthread_t t1, t2;
	pthread_create(&t1, NULL, fun1, &fds1_write);
	pthread_create(&t2, NULL, fun1, &fds1_write);


  int i = 0, x = 0;
  for(i = 0 ; i < 2; ++i) {
    FD_SET(&set, fds1_read);
    FD_SET(&set, fds2_read);
    select(fds1_read+fds2_read, &set, 0, 0);
    if(fds1_read & set) {
      printf("w00t");
    }
  }



	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}

