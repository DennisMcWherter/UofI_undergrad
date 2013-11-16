#include <stdio.h>
#include <unistd.h>

int main()
{
    int pid = getpid();
    char buf[1024];

    sprintf(buf, "echo \"R %d\" > /proc/mp3/status", pid);

    system(buf);

    sprintf(buf, "cat /proc/mp3/status");

    system(buf);

    sleep(2);

    sprintf(buf, "echo \"U %d\" > /proc/mp3/status", pid);

    system(buf);

    return 0;
}

