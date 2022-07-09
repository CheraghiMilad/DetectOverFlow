//
// Created by milad on 04/07/2022.
//
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
int wait_for_syscall(pid_t TARGET_PROGRAM)
{
    int status;
    int err = 0;

    while (1) {
        err = ptrace(PTRACE_SINGLESTEP, TARGET_PROGRAM, 0, 0);
        if (err == -1) {
            return err;
        }
        waitpid(TARGET_PROGRAM, &status, 0);

        if (WIFSTOPPED(status) && WSTOPSIG(status) && 0x80) {
            return 0;
        }
        if (WIFEXITED(status)) {
            return 1;
        }
    }
    return 0;
}