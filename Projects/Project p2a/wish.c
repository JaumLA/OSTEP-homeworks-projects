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
    struct Node *previous;
};

void getcommands(char *, struct Node *);
struct Node *executecommand(struct Node *);
void addn(struct Node *);
void freell(struct Node *);

char *commands[BUFFERSIZE];

char *path[BUFFERSIZE];
int psize = 1;

char error_message[30] = "An error has occurred\n";

int main(int argc, char const *argv[])
{
    struct Node *h;
    char *line;
    FILE *fp = stdin;
    path[0] = "/bin";
    line = malloc(BUFFERSIZE);
    size_t size = BUFFERSIZE;

    if (argc == 2) // read commands from file
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
        h = malloc(sizeof(struct Node));
        struct Node *n;
        if (fp == stdin)
        {
            printf("wish> ");
        }
        if (getline(&line, &size, fp) <= 0)
            break;

        getcommands(line, h);
        n = h;
        while ((n != NULL) && (n->comm != NULL))
            n = executecommand(n);
        freell(h);
    }

    return 0;
}

struct Node *executecommand(struct Node *h)
{
    int size = 0;
    int redirc = 0;
    struct Node *n = NULL;
    struct Node *tmp = h;
    struct Node *redirn = NULL;
    while ((tmp != NULL))
    {
        if ((strcmp(tmp->comm, "&") == 0))
        {
            n = tmp->previous;
            size--;
            break;
        }
        else if (strcmp(tmp->comm, ">") == 0)
        {
            if ((tmp->previous == NULL) || (tmp->next == NULL) || (redirc >= 1))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            redirn = tmp->next;
            redirc++;
        }

        n = tmp;
        tmp = tmp->next;
        size++;
    }
    if (n == NULL)
        return n;

    size++;
    if (redirn != NULL)
    {
        if ((redirn->next != NULL))
        {
            if (strcmp(redirn->next->comm, "&") != 0)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
        }
    }

    if (strcmp(h->comm, "exit") == 0)
    {
        if (h != n) // Check if command has more args
            write(STDERR_FILENO, error_message, strlen(error_message));

        exit(0);
    }
    else if (strcmp(h->comm, "cd") == 0)
    {
        if (h->next != n || chdir(n->comm)) // Check if command has more than 1 arg
            write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else if (strcmp(h->comm, "path") == 0)
    {
        if (h == n) // No args
        {
            path[0] = NULL;
            psize = 0;
        }
        else
        {
            int i = 0;
            psize = 0;
            struct Node *tmp = h;
            do
            {
                if ((tmp = tmp->next) == NULL)
                    exit(1);

                if (tmp->comm[0] == '/') // Is a absolute path
                {
                    path[i] = malloc(BUFFERSIZE);
                    strcpy(path[i], tmp->comm);
                }
                else // Get the absolute path
                {
                    char *completep;
                    path[i] = malloc(BUFFERSIZE);
                    completep = getcwd(NULL, 0);
                    strcat(completep, "/");
                    strcat(completep, tmp->comm);
                    strcpy(path[i], completep);
                }
                psize++;
                i++;
            } while (tmp != n);
        }
    }
    else
    {
        int rc = -1;
        char *progpath = malloc(BUFFERSIZE);
        for (int i = 0; i < psize; i++)
        {
            strcpy(progpath, path[i]);
            strcat(progpath, "/");
            strcat(progpath, h->comm);
            if (!access(progpath, X_OK))
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
            char *argv[size + 1];
            struct Node *tmp = h;
            int i = 0;
            while ((i < size) && (tmp != NULL))
            {
                argv[i] = malloc(BUFFERSIZE / 2);
                strcpy(argv[i], tmp->comm);
                i++;

                if (redirn != NULL)
                {
                    if (tmp == redirn->previous->previous)
                        break;
                }
                if (tmp == n)
                    break;

                tmp = tmp->next;
            }

            argv[i] = NULL;

            if (redirn != NULL)
            {
                int fp;
                mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                if ((fp = open(redirn->comm, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1)
                    exit(1);

                if (dup2(fp, STDOUT_FILENO) == -1)
                    exit(1);

                close(fp);
            }
            execv(progpath, argv);

            exit(1);
        }
        else
            wait(NULL);
    }
    n = n->next;
    if (n == NULL)
        return n;
    else
        return n->next;
}

/* getcommands: parse a line of commands separated by whitespaces adding each command
 into a node of the given head of a linked list. */
void getcommands(char *line, struct Node *n)
{
    int i = 0;
    char *ptr = line;
    n->previous = NULL;
    while (((commands[i] = strsep(&ptr, " ")) != NULL) && i < BUFFERSIZE / 2)
    {
        if (strlen(commands[i]) == 0 || isspace(commands[i][0]))
        {
            continue;
        }

        n->comm = malloc(BUFFERSIZE / 2);
        strcpy(n->comm, commands[i]);
        addn(n);
        n = n->next;
        i++;
    }
    if (n->previous == NULL)
        return;
    n = n->previous;
    n->next = NULL;
    if (n->comm[strlen(n->comm) - 1] == '\n')
    {
        n->comm[strlen(n->comm) - 1] = '\0';
    }
}

/* addn: allocate a new node to n->next. The next node has n in its node->previous field */
void addn(struct Node *n)
{
    struct Node *tmp = malloc(sizeof(struct Node));
    tmp->comm = NULL;
    n->next = tmp;
    tmp->previous = n;
}

/* freell: free the given linked list. */
void freell(struct Node *h)
{
    struct Node *tmp;

    while (h != NULL)
    {
        tmp = h;
        h = h->next;
        free(tmp);
    }
}