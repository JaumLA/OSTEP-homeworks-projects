#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void *pzip(void *);
int fgetsize(int);
void write_the_work(void);
void itos(int, char *);
int read_int_char(int *, char *, FILE *);
void write_int_char_to_stdout(int *, char *);

typedef struct pzarg
{
    char *mp;         // the pointer to the mmap file
    int offset;       // the start pos of the mmap
    int pos_to_write; // how many threads wait untill write to tmpout to keep the order of the files
    int mpsize;       // size of mmap mp
} pzarg;

pthread_mutex_t concl;   // mutex to concatenate work
pthread_cond_t conccond; // condition variable to concatenate work
int finishedt = 0;       // number of finished threads
int np;                  // number of processors and so of max threads
int remaining_files;

char template_name[20] = "pthread_out_";

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    pthread_mutex_init(&concl, NULL);
    pthread_cond_init(&conccond, NULL);

    np = get_nprocs();
    remaining_files = argc - 1; // number of files
    pthread_t pid[np];
    for (int i = 0; i < argc - 1; i++)
    {
        int fd = open(argv[i + 1], O_CLOEXEC | O_RDONLY);
        if (fd == -1)
        {
            printf("Can't open %s. \n", argv[i + 1]);
            exit(1);
        }
        int fsize = fgetsize(fd);
        char *mp;
        if ((mp = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
        {
            printf("mmap error.\n");
            exit(1);
        }
        // if the number of threads working excceds the number of processors
        //  wait they finish and then concatenate the resulting work of all
        // the threads.
        if ((i != 0) && (i % np == 0))
            write_the_work();
        remaining_files--;
        
        pzarg *parg = malloc(sizeof(pzarg));
        parg->mp = mp;
        parg->offset = 0;
        parg->pos_to_write = i % np;
        parg->mpsize = fsize;
        pthread_create(&pid[i % np], NULL, pzip, parg);
    }
    if(argc - 1 < np)
        write_the_work();
    return 0;
}

/* fgetsize: given a file descriptor, return its size in bytes. */
int fgetsize(int fd)
{
    long fsize;
    if ((fsize = lseek(fd, 0L, SEEK_END)) == -1)
    {
        printf("lseek error. \n");
        exit(1);
    }
    if (lseek(fd, 0L, SEEK_SET))
    {
        printf("lseek error. \n");
        exit(1);
    }
    return fsize;
}

/* npzip: compress char *nf[n] files together using RLE and
    print the result on tmpout. */
void *pzip(void *arg)
{

    char nextc = 0;
    int count = 1;
    pzarg *zarg = (pzarg *)arg;
    char *initbyte = zarg->mp + zarg->offset; // keep the initial location to check if actualbyte > mp size
    char *actualbyte = zarg->mp + zarg->offset;

    char fname[20];
    char snum[4];
    strcpy(fname, template_name);
    itos(zarg->pos_to_write, snum);
    strcat(fname, snum);
    FILE *tmpout;
    if ((tmpout = fopen(fname, "w+")) == NULL)
    {
        printf("Can't open/create tmp file for thread outup. \n");
        exit(1);
    }
    // gets the first character of the possible list
    // of files.
    int c = *actualbyte;
    while (1)
    {
        actualbyte++;

        // if there is no more files and we got EOF,
        // write the compressed form of character c, else
        // continue looking for same characters on next file.
        nextc = *actualbyte;
        if ((actualbyte - initbyte) >= zarg->mpsize)
        {
            fwrite(&count, sizeof(int), 1, tmpout);
            fwrite(&c, sizeof(char), 1, tmpout);
            c = nextc;
            break;
        }
        if (c != nextc)
        {
            fwrite(&count, sizeof(int), 1, tmpout);
            fwrite(&c, sizeof(char), 1, tmpout);
            count = 0;
        }

        c = nextc;
        count++;
    }

    // each thread work with one file, so add to the finished
    pthread_mutex_lock(&concl);
    finishedt++;
    pthread_cond_signal(&conccond);
    pthread_mutex_unlock(&concl);

    fclose(tmpout);
    free(zarg);
    exit(0);
}

/* itos: convert the int num to string into the address *s */
void itos(int num, char *s)
{
    if (sprintf(s, "%d", num) < 0)
    {
        printf("Cannot create tmp file name.\n");
        exit(1);
    }
}

/* write_the_work: wait all threads finish the compression onto the files
    pthread_out_i, where i is the number to identify the work order. Then
    write to one file correcting the files with same ending and beginning characters. */

int nextcount;
char nextc;
int count = -1;
char c = -1;

void write_the_work()
{
    pthread_mutex_lock(&concl);
    
    while ((finishedt < np) || (remaining_files > 0))
    {
        pthread_cond_wait(&conccond, &concl);
    }
    finishedt = 0;
    char actual_out[20];
    char pos[4];
    for (int i = 0; i < np; i++)
    {
        strcpy(actual_out, template_name);
        itos(i, pos);
        strcat(actual_out, pos);
        FILE *fp = fopen(actual_out, "r");

        // if this isn't the first file, se if we can concatenate the ending
        //  char c of the last file with the actual file first character
        if ((count != -1) && (c != -1))
        {
            if (read_int_char(&nextcount, &nextc, fp) == EOF)
                continue; // there is just one character, so continue to another file

            if (c == nextc)
            {
                count += nextcount;
                write_int_char_to_stdout(&count, &c); // sum the number of characters
            }
        }

        while (1)
        {
            if (read_int_char(&nextcount, &nextc, fp) == EOF)
                break; // go to another file, if there is one

            count = nextcount;
            c = nextc;
            write_int_char_to_stdout(&count, &c);
        }
    }

    // if there is files to write, we keep track of the last
    // count and char seen, to make a possible concatenation
    if (remaining_files <= 0)
        write_int_char_to_stdout(&count, &c);
    pthread_mutex_unlock(&concl);
}

/* read_int_char: take a int pointer, char pointer and a file pointer. First
    read 4 bytes into the int pointer and then 1 byte into char pointer. */
int read_int_char(int *count, char *c, FILE *fp)
{
    fread(count, sizeof(int), 1, fp);
    fread(c, sizeof(char), 1, fp);
    if ((*count == EOF) || (*c == EOF))
        return EOF;
    return 0;
}

/* write_int_char_to_stdout: take a int pointer and a char pointer. First
   write the number pointed in count and the char pointed in c into stdou, respectively. */
void write_int_char_to_stdout(int *count, char *c)
{
    if (fwrite(count, sizeof(int), 1, stdout) == -1)
    {
        printf("Error writing to stdout.\n");
        exit(1);
    }
    if (fwrite(c, sizeof(char), 1, stdout) == -1)
    {
        printf("Error writing to stdout.\n");
        exit(1);
    }
}