#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lib.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>

/*
 * Function:  parse_string
 * --------------------
 *  parses a string into an array of strings
 *
 *  string: string to be parsed
 *
 *  returns: program struct with the parsed data
 */
program parse_string(char *string)
{
    char *token = strtok(string, "#");
    char **strings = NULL;
    int i = 0, num_strings = 1;
    while (token != NULL)
    {
        strings = (char **)realloc(strings, num_strings * sizeof(char *));
        strings[i] = token;
        token = strtok(NULL, "#");
        i++, num_strings++;
    }
    program tracer;
    tracer.pid = atoi(strings[0]);
    tracer.running = atoi(strings[1]);
    tracer.status = atoi(strings[2]);
    tracer.sec = atol(strings[3]);
    tracer.ms = atol(strings[4]);
    tracer.program = strings[5];

    return tracer;
}

char **parse(char *string)
{
    char *token = strtok(string, " ");
    char **strings = NULL;
    int i = 0, num_strings = 1;
    while (token != NULL)
    {
        strings = (char **)realloc(strings, num_strings * sizeof(char *));
        strings[i] = token;
        token = strtok(NULL, " ");
        i++, num_strings++;
    }
    return strings;
}

struct timeval execOperation(char *file, char *operation, char *second_operator)
{
    pid_t pid;
    if (!(pid = fork()))
    {
        if (second_operator != NULL)
            execlp(operation, operation, second_operator, file, NULL);
        else
            execlp(operation, operation, file, NULL);
        exit(EXIT_SUCCESS);
    }
    int j;
    wait(&j);
    write(1, "\n", strlen("\n"));
    struct timeval stop;
    gettimeofday(&stop, NULL);
    return stop;
}

struct timeval pipeline(char **cmd, int num_cmds)
{

    int pipes[num_cmds - 1][2];
    int i, num_cmds2 = num_cmds;
    printf("Num cmds2: %d\n", num_cmds2);
    for (i = 0; i < num_cmds2; i++)
    {
        printf("I: %d\n", i);
        printf("Cmd: %s\n", cmd[i]);
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
            char *args = malloc(sizeof(char) * 100);
            strcpy(args, cmd[i]);
            char **args2 = parse(cmd[i]);
            if (i == 0)
            {
                close(pipes[i][0]);
                dup2(pipes[i][1], 1);
                close(pipes[i][1]);
            }
            else if (i == num_cmds2 - 1)
            {
                close(pipes[i - 1][1]);
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                printf("I: %d\n", i);
            }
            else
            {
                close(pipes[i - 1][1]);
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                dup2(pipes[i][1], 1);
                close(pipes[i][1]);
            }
            printf("Args: %s\n", args2[1]);
            execvp(args2[0], args2);
            printf("Error: %s\n", args2[0]);
            free(args2);
            _exit(0);
            break;
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        }
    }

    for (int h = 0; h < num_cmds - 1; h++)
    {
        close(pipes[h][0]);
        close(pipes[h][1]);
        int j;
        wait(&j);
    }

    struct timeval stop;
    gettimeofday(&stop, NULL);
    return stop;
}

char **parse_pipeline(char *cmd_str, int *num_args)
{

    const char *delimiters = "|";
    const size_t max_args = 1024;
    const size_t max_arg_len = 4096;

    char **args = malloc(sizeof(char *) * (max_args + 1));
    if (!args)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    const char *p = cmd_str;
    size_t i = 0;
    while (*p)
    {
        while (*p && strchr(delimiters, *p))
            ++p;
        if (!*p)
            break;
        args[i] = malloc(max_arg_len);
        if (!args[i])
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        size_t j = 0;
        while (*p && !strchr(delimiters, *p))
        {
            if (j < max_arg_len - 1)
                args[i][j++] = *p++;
            else
                ++p;
        }
        args[i][j] = '\0';
        ++i;
        if (*p == '|')
            ++p;
    }
    args[i] = NULL;

    *num_args = i;
    return args;
}
