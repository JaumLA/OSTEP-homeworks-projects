#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

/* Yes we can do it without calling wait */
int main(int argc, char const *argv[])
{
    int rc = fork();
    wait(NULL);
    if (rc == -1)
    {
        printf("Something bad occurred.\n");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("Child say: Hello.\n");
    }
    else
    {
        printf("Parent say: Goodbye.\n");
    }
    return 0;
}
