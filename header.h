//
// Created by milad on 06/07/2022.
//

#ifndef DETECTOVERFLOW_HEADER_H
#define DETECTOVERFLOW_HEADER_H


int wait_for_syscall(pid_t TARGET_PROGRAM);

int RUN_TARGET_PROGRAM();

int RUN_TRACER(pid_t TARGET_PROGRAM);
#endif //DETECTOVERFLOW_HEADER_H
