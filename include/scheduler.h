#ifndef __SCHEDULER__
#define __SCHEDULER__

#include "ucontext.h"
#include "support.h"

FILA2 *initFIFOQueue();

PRIO_QUEUE_t *initPriorityQueue();

void initSchedulerQueues();

int initMainThread();

void killThread();

int blockThread();

int unlockThread(int tid);

int scheduleNewThread();

void runThread(TCB_t *thread);

int setRunningThreadPriority(int prio);

int yield();

int waitForThread(int tid);

TCB_t *findReadyThreadByTID(int tid);

TCB_t *getRunningThread();

TCB_t *popPriorityThread(PRIO_QUEUE_t *queue);

int insertReadyQueue(TCB_t *thread);

#endif