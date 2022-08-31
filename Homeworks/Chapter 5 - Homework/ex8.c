#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

/* If wait is used in the child process it return -1. and
    terminate the child process, if used in the parent wait for,
    the child to exit and return the child pid. */
int main(int argc, char const *argv[])
{
    int rc2 = -2;
    int argp[2];
    char buff;
    if (pipe(argp) < 0)
    {
        printf("Error on pipe function.");
        exit(1);
    }
    int rc1 = fork();
    if (rc1 != 0)
    {
        rc2 = fork();
    }

    if (rc1 == -1 || rc2 == -1)
    {
        printf("Something bad occurred.\n");
        exit(1);
    }
    else if (rc1 == 0)
    {
        printf("Child1:\n");
        close(argp[0]);
        dup2(argp[1], 1);
        argp[0] = STDIN_FILENO;
        printf("Where am I?\n");
    }
    else if (rc2 == 0)
    {
        printf("Child2:\n");
        printf("Before pipe2.\n");
        close(argp[1]);
        dup2(argp[0], 0);
        argp[1] = STDOUT_FILENO;
        while (read(argp[0], &buff, 1))
        {
            write(argp[1], &buff, 1);
        }
    }
    else
    {
        printf("Parent say: Hi.\n");
    }
    return 0;
}
