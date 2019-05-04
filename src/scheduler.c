#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/thread.h"

// Fila de threads aptas a executarem organizada por prioridade
PRIO_QUEUE_t *readyQueue;

// Filas respectivas a cada um dos demais estado (Executando, Bloqueado, Terminado)
FILA2 *runningQueue, *blockedQueue, *finishedQueue;

// Fila para pares thread_pai/thread_filho para a operação JOIN
FILA2 *joinQueue;

TCB_t *mainThreadTCB;
ucontext_t mainThreadContext;


int initQueues() {
    int retStatus = 0;

    // Priority Queue (Ready)
    readyQueue = malloc(sizeof(PRIO_QUEUE_t));
    readyQueue->high = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(readyQueue->high);
    readyQueue->medium = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(readyQueue->medium);
    readyQueue->low = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(readyQueue->low);

    // State Queues
    runningQueue = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(runningQueue);
    blockedQueue = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(blockedQueue);
    finishedQueue = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(finishedQueue);

    // Join Pair Queue
    joinQueue = malloc(sizeof(PFILA2));
    retStatus -= CreateFila2(joinQueue);

    return retStatus;
}

int initMainThread() {
    mainThreadTCB = malloc(sizeof(TCB_t));

    initQueues();

    getcontext(&mainThreadContext);
    mainThreadTCB = createThread(mainThreadContext, LOW_PRIORITY);

    if(mainThreadTCB != NULL){
        mainThreadTCB->state = PROCST_EXEC;
        AppendFila2(runningQueue, mainThreadTCB);
        return 0;
    }else{
        return -1;
    }
}

int insertReadyQueue(TCB_t *thread){
    thread->state = PROCST_APTO;

    switch(thread->prio) {
        case HIGH_PRIORITY:
            AppendFila2(readyQueue->high, thread);
            break;
        case MEDIUM_PRIORITY:
            AppendFila2(readyQueue->medium, thread);
            break;
        case LOW_PRIORITY:
            AppendFila2(readyQueue->low, thread);
            break;
        default:
            return -1;
    }

    return 0;
}

void runThread(TCB_t *thread) {
    thread->state = PROCST_EXEC;
    AppendFila2(runningQueue, thread);
    setcontext(&(thread->context));
}

int scheduleNewThread() {
    TCB_t *thread = malloc(sizeof(TCB_t));

    // Verifica se existe alguma thread de prioridade alta
    if(FirstFila2(readyQueue->high) == 0){
        thread = GetAtAntIteratorFila2(readyQueue->high);
        runThread(thread);
    } else if(FirstFila2(readyQueue->medium) == 0){
        thread = GetAtAntIteratorFila2(readyQueue->medium);
        runThread(thread);
    } else if(FirstFila2(readyQueue->low) == 0){
        thread = GetAtAntIteratorFila2(readyQueue->low);
        runThread(thread);
    } else
        return -1;

    return 0;
}

int unlockThread(int tid) {
    // Percorre a fila de threads bloqueadas
    TCB_t *thread;
    FirstFila2(blockedQueue);
    do{
        thread = (TCB_t*) GetAtAntIteratorFila2(blockedQueue);
        if(thread == NULL)
            break;
        if(thread->tid == tid){
            // Se encontrar a thread, transfera da fila bloqueados para a fila aptos
            DeleteAtIteratorFila2(blockedQueue);
            thread->state = PROCST_APTO;
            insertReadyQueue(thread);
            return 0;
        }
    } while(NextFila2(blockedQueue) == 0);

    return -1;
}

void killThread() {
    // Identifica a thread que está em execução
    TCB_t *thread = malloc(sizeof(TCB_t));
    thread = (TCB_t*) GetAtAntIteratorFila2(runningQueue);
    // Remove a thread da fila "executando"
    DeleteAtIteratorFila2(runningQueue);
    // Atualiza o estado da thread
    thread->state = PROCST_TERMINO;
    
    // Verifica se existe alguma thread esperando pela thread finalizada
    JOIN_PAIR_t *joinPair;
    FirstFila2(joinQueue);
    do{
        joinPair = (JOIN_PAIR_t*) GetAtAntIteratorFila2(joinQueue);
        if(joinPair == NULL)
            break;
        if(joinPair->tid_running_thread == thread->tid)
            unlockThread(joinPair->tid_blocked_thread);
    }while(NextFila2(joinQueue) == 0);

    AppendFila2(finishedQueue, thread);
    scheduleNewThread();
}

int setRunningThreadPriority(int prio){
    TCB_t *thread = GetAtAntIteratorFila2(runningQueue);
    if(thread != NULL){
        thread->prio = prio;
        return 0;
    } else
        return -1;
    
}



