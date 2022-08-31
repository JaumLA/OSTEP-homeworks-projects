#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERSIZE 512

struct Node
{
    char *comm;
    struct Node *next;
};

int getcommands(char *);
void executecommand();
int getredirec();

char *commands[BUFFERSIZE];
char *path[BUFFERSIZE];

int doredi = 0;

int psize = 1;
int commindex = 0;
int commsize = 0;

char error_message[30] = "An error has occurred\n";

int main(int argc, char const *argv[])
{
    char *line;
    FILE *fp = stdin;
    path[0] = "/bin";
    line = malloc(BUFFERSIZE);
    size_t size = BUFFERSIZE;

    if (argc == 2)
    {
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
    }
    else if (argc > 2)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    while (1)
    {
        if (fp == stdin)
        {
            printf("wish> ");
        }
        if (getline(&line, &size, fp) <= 0)
        {
            break;
        }
        commsize = getcommands(line);
        while (commindex < commsize)
        {
            executecommand();
        }
        commindex = 0;
        commsize = 0;
    }

    return 0;
}

void executecommand()
{
    int finalindex = commindex;
    int retindex = 0;
    int idire = 0;

    // get size of command + flags.
    while (finalindex < commsize && (strcmp(commands[finalindex], "&")))
    {
        if (strcmp(commands[finalindex], ">") == 0)
        {
            if (getredirec(finalindex) == -1)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                commindex = commsize;
                return;
            }

            idire = finalindex;
            retindex = idire + 3;
            break;
        }
        finalindex++;
    }

    char **flags;
    int flagc = finalindex - commindex - 1;
    flags = commands + 1;
    if (!strcmp(commands[commindex], "exit"))
    {
        if (flagc != 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        exit(0);
    }
    else if (!strcmp(commands[commindex], "cd"))
    {
        if (flagc != 1 || chdir(flags[0]))
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
    else if (!strcmp(commands[commindex], "path"))
    {
        if (flagc == 0)
        {
            path[0] = NULL;
            psize = 0;
        }
        else
        {
            int i = 0;
            psize = 0;
            while ((i < flagc) && (i < (BUFFERSIZE / 2)))
            {
                if (flags[i][0] == '/') // Is a absolute path
                {
                    path[i] = malloc(BUFFERSIZE);
                    strcpy(path[i], flags[i]);
                }
                else // Get the absolute path
                {
                    char *completep;
                    path[i] = malloc(BUFFERSIZE);
                    completep = getcwd(NULL, 0);
                    strcat(completep, "/");
                    strcat(completep, flags[i]);
                    strcpy(path[i], completep);
                }
                psize++;
                i++;
            }
        }
    }
    else if (commindex < finalindex)
    {
        int rc = -1;
        char *commpath = malloc(BUFFERSIZE);
        for (int i = 0; i < psize; i++)
        {
            strcpy(commpath, path[i]);
            strcat(commpath, "/");
            strcat(commpath, commands[commindex]);
            if (!access(commpath, X_OK))
            {
                rc = fork();
                break;
            }
        }
        if (rc == -1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else if (rc == 0)
        {
            int argc = flagc + 1;
            char *argv[argc + 1];
            argv[0] = commands[commindex];
            argv[argc] = NULL;
            for (int i = 0; i < flagc; i++)
            {
                argv[i + 1] = flags[i];
            }
            if (doredi)
            {
                int fp;
                mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                if ((fp = open(commands[idire + 1], O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1)
                {
                    exit(1);
                }
                if (dup2(fp, STDOUT_FILENO) == -1)
                {
                    exit(1);
                }
                close(fp);
            }

            execv(commpath, argv);

            // write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        else
        {
            wait(NULL);
        }
    }

    if (retindex != 0)
        commindex = retindex;
    else
        commindex = finalindex + 1;
    doredi = 0;
}

int getcommands(char *line)
{
    int i = 0;
    char *ptr = line;
    struct Node *head = malloc(sizeof(struct Node));
    struct Node *n = malloc(sizeof(struct Node));
    head->next = n;
    while (((commands[i] = strsep(&ptr, " ")) != NULL) && i < BUFFERSIZE / 2)
    {
        if (strlen(commands[i]) == 0 || isblank(commands[i][0]))
        {
            continue;
        }
        if (head->comm == NULL)
        {
            head->comm = malloc(BUFFERSIZE/2);
            strcpy(head->comm, commands[i]);
        }
        else
        {
            n->comm = malloc(BUFFERSIZE/2);
            strcpy(n->comm, commands[i]);
            struct Node *tmp = malloc(sizeof(struct Node));
            n->next = tmp;
            n = tmp;
            n->comm = NULL;
        }
        i++;
    }
    i--;
    if (commands[i][strlen(commands[i]) - 1] == '\n')
    {
        commands[i][strlen(commands[i]) - 1] = '\0';
    }
    if (strlen(commands[i]) == 0)
    {
        i--;
    }

    return i + 1;
}

int getredirec(int initial)
{
    int i = initial;
    if ((i == 0) || (i == commsize - 1))
    {
        doredi = 0;
        return -1;
    }
    int finali = i;
    while ((finali < commsize) && strcmp(commands[finali], "&"))
        finali++;
    if (finali - i - 1 != 1)
    {
        doredi = 0;
        return -1;
    }
    doredi = 1;
    return 0;
}