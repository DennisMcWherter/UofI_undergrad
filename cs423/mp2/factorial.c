/**
 * Example.c
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define BUFSIZE 4096

static FILE* fp = NULL;

int reg(int period, int computation)
{
    int pid = getpid();
    int bytes = 0;
    int lpid = 0, lperiod = 0, lcomp = 0;
    char buffer[BUFSIZE];
    int eof = 3;
    int offset = 0;
    char * fbuffer = buffer;

    fprintf(fp, "R, %d, %d, %d", pid, period, computation);
    fflush(fp);

    printf("reading procfs\n");
    rewind(fp);
    bytes = fread(buffer, 1, BUFSIZE-1, fp);
    buffer[bytes] = '\0';
    printf("procfs contents: %s\n", buffer);

    while((eof != EOF)) {
        eof = sscanf(fbuffer, "%d,%d,%d;%n", &lpid, &lperiod, &lcomp, &offset);
        fbuffer += offset;
        printf("Scanning: lpid: %d, lperiod: %d, lcomp: %d\n", lpid, lperiod, lcomp);
        if(lpid == pid) {
            printf("Registered: lpid: %d, pid: %d\n", lpid, pid);
            return 1;
        }
        if(eof == EOF) {
            return 0;
        }
    }

    return 0;
}

void yield()
{
    int pid = getpid();
    char buf[1024];
    int bytes = sprintf(buf, "Y, %d", pid);
    buf[bytes] = '\0';
    fwrite(buf, 1, bytes+1, fp); 
    fflush(fp);
    printf("yielding: %d\n", pid);
}

void deregister()
{
    int pid = getpid();
    char buf[1024];
    int bytes = sprintf(buf, "D, %d", pid);
    buf[bytes] = '\0';
    fwrite(buf, 1, bytes+1, fp); 
    fflush(fp);
    printf("De-registering: %d\n", pid);
}

void spawnInstances(int x)
{
    int current = 0;
    int i = 0;

    printf("Spawning %d instances...\n", x);

    for(i = 0 ; i < x ; ++i) {
        if(!fork())
            break;
    }
}

int factorial(float n)
{
    if(n == 0.0)
        return 0;
    return n * factorial(n - 1);
}

int main(int argc, char** argv)
{
    int i = 0;
    int j = 0;
    struct timeval t0;
    struct timeval tcur;
    struct timeval wakeup;
    int wakeup_time = 0;
    int period, computation, regi;
    period = 0;
    computation = 0;
    regi = 0;

    //spawnInstances(2);

    fp = fopen("/proc/mp2/status", "w+");
    if(fp == NULL) {
        printf("Could not open /proc/mp2/status\n");
        return -1;
    }

    printf("I am: %d\n", getpid());

    period = atoi(argv[1]);
    computation = atoi(argv[2]);
    printf("period: %d, comp: %d\n", period, computation);
    
    regi = reg(period, computation);
    printf("Reg: %d\n", regi);
    fflush(stdout);
    if(!regi) {
        fprintf(stderr, "Could not register process!\n");
        fflush(stderr);
        fclose(fp);
        return 1;
    }
    printf("p/1k: %d\n", (period/1000));
    gettimeofday(&t0, NULL);
    yield();

    for(i = 0 ; i < 30 ; ++i) {
        printf("i: %d\n", i);
        wakeup_time = t0.tv_sec + ((i+1) * (period / 1000));
        wakeup.tv_sec = wakeup_time;
        printf("Wakeup time (%d): %d\n", getpid(), wakeup_time);
        gettimeofday(&tcur, NULL);
        while(wakeup.tv_sec != tcur.tv_sec) {
            factorial(100000);
            gettimeofday(&tcur, NULL);
        }
        if (i < 30)
            yield();
    }
  
    deregister();

    fclose(fp);

    return 0;
}

