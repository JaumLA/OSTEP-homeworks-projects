#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fwgrep(FILE *, char *);
void sigrep();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    if (argc == 2)
    {
        sigrep(argv[1]);
    }
    else
    {
        FILE *fp = fopen(argv[2], "r");
        if (fp == NULL)
        {
            printf("wgrep: cannot open file\n");
            exit(1);
        }
        fwgrep(fp, argv[1]);
        fclose(fp);
    }
    return 0;
}

#define BUFFERSIZE 1024 // Size of the line buffer

size_t bp = BUFFERSIZE;
char *buffer;

/* fwgrep: if a line of the file fp contain the substr, print it*/
void fwgrep(FILE *fp, char *substr)
{
    while (getline(&buffer, &bp, fp) > 0)
    {
        if (strstr(buffer, substr) != NULL)
        {
            printf("%s", buffer);
        }
    }
}

/*sigrep: reads from the standard input*/
void sigrep(char *substr)
{
    buffer = malloc(BUFFERSIZE);
    while (fgets(buffer, BUFFERSIZE, stdin) > 0)
    {
        if (strstr(buffer, substr) != NULL)
        {
            printf("%s", buffer);
        }
    }
}
