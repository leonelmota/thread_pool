#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <time.h>

typedef struct {
    int pid;
    int ms;
} task_descr_t;

void processa(task_descr_t* tp){
    struct timespec zzz;

    zzz.tv_sec  = tp->ms/1000;
    zzz.tv_nsec = (tp->ms%1000) * 1000000L; 

    printf("IP #%d\n", tp->pid);
    nanosleep(&zzz,NULL);
    printf("FP #%d\n", tp->pid);
}

#endif