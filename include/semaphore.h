#ifndef __SEMAPHORE__
#define __SEMAPHORE__

#include "support.h"
#include "thread.h"

int waitForResource(csem_t *sem);

int unlockSemThread(csem_t *sem);

#endif