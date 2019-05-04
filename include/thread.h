#ifndef __thread__
#define __thread__

#include <signal.h>

#define STACK_SIZE SIGSTKSZ

#define HIGH_PRIORITY 0
#define MEDIUM_PRIORITY 1
#define LOW_PRIORITY 2

void createContext(ucontext_t* context, void* (*start)(void*), void* arg);

TCB_t* createThread(ucontext_t context, int priority);

#endif