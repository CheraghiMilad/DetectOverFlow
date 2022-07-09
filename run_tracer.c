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
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <elf.h>
#include <stdarg.h>

#include "header.h"

int RUN_TRACER(pid_t TARGET_PROGRAM) {
    int err = 0;
    int status;
    int counter = 0;
    long Opcode, Opcode2;
    unsigned char primary_Opcode;
    unsigned char secondary_Opcode;
    long long bfr[100000];
    int OPCODE_RET_Happend;
    /* Waiting for happend systemcall in child process.*/
    err = waitpid(TARGET_PROGRAM, &status, 0);
    if (err == -1) {return err;}
    typedef struct handle {
        struct user_regs_struct regs;
        long long NextInstruction;
        long long ReturnAddress;
        long long CheckRet;
    } handle_t;
    handle_t handler;

    printf("\n**Start Analysis Process by PID:%d\n", getpid());
    err = ptrace(PTRACE_SETOPTIONS, TARGET_PROGRAM, 0, PTRACE_O_TRACESYSGOOD); //     Set Option
    if (err == -1) {
        return err;
    }
    int OPCODE_CALL_Happend = 0;


    /* This is a loop that examine instructions accured in child process and if its a sytstem call
     * continue instruction and else waiting for another signal in child process.*/
    while (1)
    {
        if (wait_for_syscall(TARGET_PROGRAM) != 0)
            break;
        ptrace(PTRACE_GETREGS, TARGET_PROGRAM, NULL, &handler.regs); // Get All Registers before execute system call instruction.
        handler.NextInstruction = ptrace(PTRACE_PEEKTEXT, TARGET_PROGRAM,
                                 handler.regs.rip,NULL); // Get Register RIP for detect what system call is calling.
/*
        int syscall = ptrace(PTRACE_PEEKUSER, TARGET_PROGRAM, sizeof(long)*ORIG_RAX);
        fprintf(stderr, "syscall(%d:%llx) \n ", syscall, handler.regs.orig_rax);
*/
        if(OPCODE_CALL_Happend == 1)
        {
            handler.ReturnAddress = ptrace(PTRACE_PEEKDATA, TARGET_PROGRAM, handler.regs.rsp, NULL); // Get RSP Register Value (Return Address is store in Address).
            bfr[counter] = handler.ReturnAddress; // Save Return Address.
            printf("\033[22;32m  System CALL Accure and Return address is: [%llx]\n \033[0m", bfr[counter]);
            counter++;
        }
        OPCODE_CALL_Happend = 0;
        primary_Opcode   = (unsigned)0xFF & handler.NextInstruction ;
        secondary_Opcode = ((unsigned)0xFF00 & handler.NextInstruction) >> 8;

        if (primary_Opcode == 0xe8 || primary_Opcode == 0xCC || primary_Opcode == 0xCD || primary_Opcode == 0xCF || (primary_Opcode == 0x0F && secondary_Opcode == 0x05))//|| Opcode == 0x9a || Opcode == 0xff || Opcode2 == 0xff00 )
        { /* Check is Opcode equal with Syscall CALL in Assembly. || Opcode == 0xCD || Opcode == 0xF1 || Opcode == 0xCC || Opcode == 0xCF
            therefore we must need understand what is next instruction? we will put condition here because here is that single step before next instruction.  */
            OPCODE_CALL_Happend = 1;
            //ptrace(PTRACE_POKEDATA, TARGET_PROGRAM,handler.regs.rsp, 333);
        }

        if (primary_Opcode == 0xc3  || primary_Opcode == 0x0F && secondary_Opcode == 0x07 || primary_Opcode == 0x0F && secondary_Opcode == 0x34){//|| Opcode == 0xc2 || Opcode == 0xca || Opcode == 0xcb || Opcode == 0xcf) { // Check it Opcode equal with Syscall RET in Assembly?
            counter--;
            handler.CheckRet = ptrace(PTRACE_PEEKDATA, TARGET_PROGRAM, handler.regs.rsp, 0);//val in curr esp stack
            OPCODE_RET_Happend = 0; //
            for (int i = counter; i >= 0; i--) {
                if (handler.CheckRet == bfr[i]) {
                    OPCODE_RET_Happend = 1;
                    printf("\033[22;34m The Return Address is valid. %llx:%llx \n \033[0m", handler.CheckRet,bfr[i]);
                    break;
                }
            }
            if (OPCODE_RET_Happend != 1) // !=1
            {
                kill(TARGET_PROGRAM, SIGKILL);
                printf("\033[22;31m Detect OverFlow >> ** KILL PROCESS ** \n The Return Address[%llx] is not valid. \033[0m",
                       handler.CheckRet);
                exit(0);
            }
        }
    }
    return 0;
}