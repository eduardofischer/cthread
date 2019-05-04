#ifndef __scheduler__
#define __scheduler__

#include "ucontext.h"
#include "support.h"
#include "thread.h"

int initQueues();

int initMainThread();

int insertPriorityQueue(PRIO_QUEUE_t *queue, TCB_t *thread);

int insertReadyQueue(TCB_t *thread);

void killThread();

int blockThread();

int unlockThread(int tid);

int scheduleNewThread();

void runThread(TCB_t *thread);

int setRunningThreadPriority(int prio);

int yield();

int waitForThread(int tid);

int waitForResource(csem_t *sem);

int unlockSemThread(csem_t *sem);

TCB_t *findReadyThreadByTID(int tid);

TCB_t *popPriorityThread(PRIO_QUEUE_t *queue);

PRIO_QUEUE_t *initPriorityQueue();

#endif