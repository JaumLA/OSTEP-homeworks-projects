#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void measuresystemcall(int);

int main(int argc, char const *argv[])
{
    measuresystemcall(1000000);
    return 0;
}

/* measuresystemcall: use gettimeofday before a loop of n calls
    to the write function and take the difference with
     the time after the loop divided by n */
void measuresystemcall(int n)
{
    struct timeval timebefore = {0};
    struct timeval timeafter = {0};
    int fd = open("./1.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    gettimeofday(&timebefore, NULL);
    for (int i = 0; i <= n; i++)
    {
        read(fd, NULL, 0);
    }
    gettimeofday(&timeafter, NULL);
    double microsec = (timeafter.tv_sec * 1000000 + timeafter.tv_usec - timebefore.tv_sec * 1000000 - timebefore.tv_usec) / (double)n;
    printf("Microseconds: %f\n", microsec);
    close(fd);
}