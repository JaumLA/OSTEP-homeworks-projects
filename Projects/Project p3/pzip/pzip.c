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
long fdgetsize(int);
long fpgetsize(FILE *);
void write_the_work(int);
void itos(int, char *);
int read_int_char(int *, char *, FILE *);
void write_int_char_to_stdout(int *, char *);

typedef struct pzarg
{
    char *mp;         // the pointer to the mmap file
    int offset;       // the start pos of the mmap for shared file
    int pos_to_write; // how many threads wait untill write to tmpout to keep the order of the files
    int byte_limit;   // how many bytes to read
} pzarg;

pthread_mutex_t concl;   // mutex to concatenate work
pthread_cond_t conccond; // condition variable to concatenate work
int finishedt = 0;       // number of finished threads
int remaining_files;

char template_name[20] = "pthread_out_";

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    if (pthread_mutex_init(&concl, NULL) != 0)
    {
        printf("Error initializing mutex.\n");
        exit(1);
    }
    if (pthread_cond_init(&conccond, NULL) != 0)
    {
        printf("Error initializing condition variable.\n");
        exit(1);
    }
    int np = get_nprocs();      // number of processors and so of max threads
    remaining_files = argc - 1; // number of files
    pthread_t pid[np];
    int totalt = 0; // total threads created
    for (int i = 0; i < argc - 1; i++)
    {
        int fd = open(argv[i + 1], O_CLOEXEC | O_RDONLY);
        if (fd == -1)
        {
            printf("Can't open %s. \n", argv[i + 1]);
            exit(1);
        }
        int fsize = fdgetsize(fd);
        char *mp;
        if ((mp = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
        {
            printf("mmap error.\n");
            exit(1);
        }
        // if the number of threads working excceds the number of processors
        // wait they finish and then concatenate the resulting work of all
        // the threads.
        if ((i != 0) && (i % np == 0))
        {
            write_the_work(totalt);
            totalt = 0;
        }
        remaining_files--;

        pzarg *parg;
        int pos = i % np;
        int threads_to_create = np - totalt; // how many threads can create
        int limit = fsize;
        int rest = 0;
        if (threads_to_create != 0)
            rest = fsize % threads_to_create; // for correcting the division of fsize

        // divide the remaining file to compress between threads
        if (remaining_files == 0)
            limit = fsize / threads_to_create;

        int start_byte_mult = 0; // keep track of starting byte
        do
        {
            totalt++;
            parg = malloc(sizeof(pzarg));
            parg->mp = mp;
            parg->byte_limit = limit;
            parg->offset = start_byte_mult * limit;
            parg->pos_to_write = pos;
            if (threads_to_create == 1)
                parg->byte_limit += rest;
            pthread_create(&pid[pos], NULL, pzip, parg);
            pos++;
            start_byte_mult++;
            threads_to_create--;
        } while ((remaining_files == 0) && (threads_to_create > 0));
    }
    if (argc - 1 % (np + 1) != 0)
        write_the_work(totalt);

    return 0;
}

/* fdgetsize: given a file descriptor, return its size in bytes. */
long fdgetsize(int fd)
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

/* pzip: compress n bytes of given file mapped on memory
    starting at offset and print the result on tmpout. */
void *pzip(void *arg)
{
    char nextc = 0;
    int count = 1;
    pzarg *zarg = (pzarg *)arg;
    // char *initbyte = zarg->mp + zarg->offset; // keep the initial location to check if actualbyte > mp size
    char *actualbyte = zarg->mp + zarg->offset;
    int totalread = 0;
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
        totalread++;
        if (totalread >= zarg->byte_limit)
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
    fclose(tmpout);
    free(zarg);
    pthread_cond_signal(&conccond);
    pthread_mutex_unlock(&concl);

    pthread_exit(NULL);
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

/* write_the_work: wait all totalt threads finish the compression onto the files
    pthread_out_i, where i is the number to identify the work order. Then
    write to one file correcting the files with same ending and beginning characters. */

void write_the_work(int totalt)
{
    if (pthread_mutex_lock(&concl) == -1)
    {
        printf("Mutex error on write_the_work.\n");
        exit(1);
    }

    while ((finishedt < totalt))
    {
        if (pthread_cond_wait(&conccond, &concl) != 0)
            exit(1);
    }
    finishedt = 0;
    char actual_out[20];
    char pos[4];

    char lastc = 0;
    int lastcount = 0;
    char firstc = -1;
    int firstcount = -1;
    for (int i = 0; i < totalt; i++)
    {
        strcpy(actual_out, template_name);
        itos(i, pos);
        strcat(actual_out, pos);
        FILE *fp;
        if ((fp = fopen(actual_out, "r")) == NULL)
        {
            printf("Can't open %s.", actual_out);
            exit(1);
        }

        int nextcount = 0;
        char nextc = 0;
        int count = 0;
        char c = 0;

        long size = fpgetsize(fp);
        if (size < 5)
            continue;
        read_int_char(&firstcount, &firstc, fp);

        // check the case when there is multiple files with one type of character
        // between the files. EX file1 = 2t, file2 = 5t.
        if (size == 5)
        {
            // add count and go to another file
            if (firstc == lastc)
            {
                lastcount += firstcount;
                continue;
            }
        }
        rewind(fp);

        // just write the last character of last file, already
        // checked
        if (lastc != 0)
            write_int_char_to_stdout(&lastcount, &lastc);
        while (1)
        {
            if (read_int_char(&nextcount, &nextc, fp) == EOF)
                break;

            count = nextcount;
            c = nextc;

            // if it's the last character, skip writing to check the first char
            // of the possible next file.
            if (read_int_char(&nextcount, &nextc, fp) == EOF)
                break;
            fseek(fp, -5, SEEK_CUR);

            write_int_char_to_stdout(&count, &c);
        }
        lastc = c;
        lastcount = count;
        fclose(fp);
    }
    write_int_char_to_stdout(&lastcount, &lastc);
    pthread_mutex_unlock(&concl);
}

/* fpgetsize: returns the size of the given FILE pointed by fp. */
long fpgetsize(FILE *fp)
{
    int size;
    if (fseek(fp, 0L, SEEK_END) == -1)
    {
        printf("fseek error.\n");
        exit(1);
    }
    if ((size = ftell(fp)) == -1)
    {
        printf("ftell error.\n");
        exit(1);
    }
    if (fseek(fp, 0L, SEEK_SET) == -1)
    {
        printf("fseek error.\n");
        exit(1);
    }
    return size;
}

/* read_int_char: take a int pointer, char pointer and a file pointer. First
    read 4 bytes into the int pointer and then 1 byte into char pointer. */
int read_int_char(int *count, char *c, FILE *fp)
{
    if (fread(count, sizeof(int), 1, fp) <= 0)
        return EOF;
    if (fread(c, sizeof(char), 1, fp) <= 0)
        return EOF;
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