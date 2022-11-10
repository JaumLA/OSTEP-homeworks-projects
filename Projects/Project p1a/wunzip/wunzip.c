#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void wunzip(FILE *);

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    for(int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        wunzip(fp);
    }
    return 0;
}

/* wunzip: get a FILE pointer fp that was zipped 
 * using run-length encoding (RLE) and unzip it.*/
void wunzip(FILE *fp){
    int times;
    char c;
    while(fread(&times, sizeof(int), 1, fp) <= 1){
        if((c = fgetc(fp)) == EOF){
            return;
        }
        for(; times > 0; times--){
            printf("%c",c);
        }
    }
}
