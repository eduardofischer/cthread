#include <stdio.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/scheduler.h"

int waitForResource(csem_t *sem){
    // Thread em execução
    TCB_t *thread = getRunningThread();

    // Verifica se a thread a ser bloqueada existe
    if(thread == NULL)
        return -1;

    // Verifica se o semáforo existe
    if(sem == NULL || sem->fila == NULL)
        return -2;
 
    // Adiciona à fila do semáforo
    AppendFila2(sem->fila, &(thread->tid));

    blockThread();
    return 0;
}

int unlockSemThread(csem_t *sem){
    int *tid = malloc(sizeof(int));
    FirstFila2(sem->fila);
    tid = GetAtIteratorFila2(sem->fila);
    DeleteAtIteratorFila2(sem->fila);

    return unlockThread(*tid);
}