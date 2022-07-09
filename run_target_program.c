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

int RUN_TARGET_PROGRAM()
{
    int err = 0;
    err = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("/home/milad/CLIONprojects/t"
          ""
          "est1", "test1", NULL);
    if (err == -1) {
        return err;
    }
    printf("Start Program that will be traced. PID:%d\n", getpid());

    err = kill(getpid(), SIGSTOP);
    if (err == -1) {
        return err;
    }
    return err;
}

