#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void wzip(FILE *, int *);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    int fnumbers = argc - 1;
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            exit(1);
        }
        wzip(fp, &fnumbers);
        fclose(fp);
    }

    return 0;
}

/* wzip: read characters of the FILE fp, compress them using RLE
    into the stdout */
char c = -1;
char nextc = 0;
int count = 1;
void wzip(FILE *fp, int *fcount)
{
    (*fcount)--;
    if (c == -1)
    {
        c = fgetc(fp);
    }
    while (1)
    {
        if ((nextc = fgetc(fp)) == EOF)
        {
            if (*fcount == 0)
            {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&c, sizeof(char), 1, stdout);
                c = nextc;
            }
            break;
        }
        if (c != nextc)
        {
            fwrite(&count, sizeof(int), 1, stdout);
            fwrite(&c, sizeof(char), 1, stdout);
            count = 0;
        }

        c = nextc;
        count++;
        continue;
    }
}