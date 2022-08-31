#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/* The two process writes into the same file */
int main(int argc, char const *argv[])
{
    int fl = open("txt2.txt", O_WRONLY);
    if(fl == -1){
        printf("Can't open the file.\n");
        exit(1);
    }
    int rc = fork();

    if (rc == -1)
    {
        printf("Something bad occurred.\n");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("Child file descriptor: %d\n", fl);
        write(fl, "Child writer.", 20);
    }
    else
    {
        printf("Parent file descriptor: %d\n", fl);
        write(fl, "Parent writer.", 20);
    }
    return 0;
}
