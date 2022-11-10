#include <stdio.h>
#include <stdlib.h>

void wcat(FILE *);

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        exit(0);
    }
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }
        wcat(fp);
        fclose(fp);
    }

    return 0;
}

#define BUFFERSIZE 512 // Maximum size of the buffer
char buffer[BUFFERSIZE];

/*wcat: print the contents of the given file.*/
void wcat(FILE *fp)
{
    while (fgets(buffer, BUFFERSIZE, fp) != NULL)
    {
        printf("%s", buffer);
    }
}
