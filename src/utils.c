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

