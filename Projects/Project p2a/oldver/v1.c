#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFERSIZE 512

int getcommands(char *);
void executecommand();

char *commands[BUFFERSIZE / 2];
char *path[BUFFERSIZE / 2];

int psize = 1;
int commindex = 0;
int commsize = 0;

char error_message[30] = "An error has occurred\n";

int main(int argc, char const *argv[])
{
    char *line;
    FILE *fp = NULL;
    path[0] = "/bin";
    line = malloc(BUFFERSIZE);
    size_t size = BUFFERSIZE;

    if (argc == 2)
    {
        fp = fopen(argv[1], "r");
    }
    if (fp != NULL)
    {
        while (getline(&line, &size, fp) != -1)
        {
            commsize = getcommands(line);
            while (commindex < commsize)
            {
                executecommand();
            }
            commindex = 0;
        }
    }
    else
    {
        while (1)
        {
            printf("wish>$ ");
            getline(&line, &size, stdin);
            commsize = getcommands(line);
            while (commindex < commsize)
            {
                executecommand();
            }
            commindex = 0;
        }
    }

    return 0;
}

void executecommand()
{
    int finalindex = commindex;
    while (finalindex < commsize && (strcmp(commands[finalindex], "&")))
        finalindex++;

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
    else
    {
        int rc = -1;
        // char *currentdir = getcwd(NULL, 0);
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
            if (execv(commpath, argv))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            }
        }
        else
        {
            wait(NULL);
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
