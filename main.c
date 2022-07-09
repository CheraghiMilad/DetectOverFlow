#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include "header.h"

int main(int argc, char* argv[])
{
    if (argc > 1) {
        fprintf(stderr, "usage: <program name> without params\n");
        return 1;
    }

    pid_t TARGET_PROGRAM = fork();

    switch (TARGET_PROGRAM) {
        case -1:
            perror("fork");
            fprintf(stderr, "create fork process failed.");
            exit(1);
        case 0:
            RUN_TARGET_PROGRAM();
            break;
        default:
            RUN_TRACER(TARGET_PROGRAM);
            break;
    }
    return 0;
}


