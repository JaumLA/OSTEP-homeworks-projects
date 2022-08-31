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

int getcommands(char *);
void executecommand();
int getredirec();

char *commands[BUFFERSIZE / 2];
char *path[BUFFERSIZE / 2];

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
        if (getredirec() != -1)
        {
            while (commindex < commsize)
            {
                executecommand();
            }
        }
        else
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        commindex = 0;
        commsize = 0;
    }

    return 0;
}

void executecommand()
{
    int finalindex = commindex;
    int idire = 0;

    // get size of command + flags.
    while (finalindex < commsize && (strcmp(commands[finalindex], "&")))
    {
        if (strcmp(commands[finalindex], ">"))
        {
            idire = finalindex;
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
            psize = 1;
            while ((i < flagc) && (i < (BUFFERSIZE / 2)))
            {
                if (flags[i][0] == '/')
                {
                    path[i] = flags[i];
                }
                else
                {
                    char *completep;
                    completep = getcwd(NULL, 0);
                    strcat(completep, "/");
                    strcat(completep, flags[i]);
                    path[i] = completep;
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
                if ((fp = open(commands[idire + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
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
            doredi = 0;
        }
    }

    commindex = finalindex + 1;
}

int getcommands(char *line)
{
    int i = 0;
    char *ptr = line;
    while (((commands[i] = strsep(&ptr, " ")) != NULL) && i < BUFFERSIZE / 2)
    {
        if (strlen(commands[i]) == 0 || isblank(commands[i][0]))
        {
            continue;
        }
        i++;
    }
    i--;
    if (commands[i][strlen(commands[i]) - 1] == '\n')
    {
        commands[i][strlen(commands[i]) - 1] = '\0';
    }
    return i + 1;
}

int getredirec()
{
    int count = 0;
    int iof = 0;
    int i = 0;
    while ((i < commsize))
    {
        if (!strcmp(commands[i], ">"))
        {
            iof = i;
            count++;
        }
        i++;
    }

    if (count == 0)
    {
        return 0;
    }
    else if (doredi)
    {
        return -1;
    }
    else if ((iof == 0) || (iof == commsize - 1) || (count > 1))
    {
        return -1;
    }
    else
    {
        int finali = iof;
        while ((finali < commsize) && strcmp(commands[finali], "&"))
            finali++;
        if (finali - iof - 1 == 1)
        {
            doredi = 1;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}