/**
 * Example.c
 */

#include <stdio.h>
#include <unistd.h>

int factorial(float n)
{
    if(n == 0.0)
        return 0;
    return n * factorial(n - 1);
}

int main()
{
    FILE* fp = fopen("/proc/mp1/status", "a");
    int i = 0;

    if(fp == NULL) {
        printf("Could not open /proc/mp1/status\n");
        return -1;
    }

    printf("I am: %d\n", getpid());
    fprintf(fp, "%d", getpid());
    fclose(fp);

    for(i = 0 ; i < 5000 ; ++i)
        factorial(200000);

    return 0;
}

