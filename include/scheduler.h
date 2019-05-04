#ifndef __scheduler__
#define __scheduler__

#include "ucontext.h"
#include "support.h"
#include "thread.h"

int initQueues();

int initMainThread();

int insertReadyQueue(TCB_t *thread);

void killThread();

int unlockThread(int tid);

int scheduleNewThread();

void runThread(TCB_t *thread);

int setRunningThreadPriority(int prio);

#endif