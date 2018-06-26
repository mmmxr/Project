#pragma once

#define MAX 1024
#define ARGS 8
#define PIPEMAX 8

typedef struct command_{
    char* argv[ARGS];
    int infd;
    int outfd;
}command__t;
