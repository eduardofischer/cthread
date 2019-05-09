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

FILA2 *initFIFOQueue(){
    FILA2 *queue = malloc(sizeof(FILA2));
    CreateFila2(queue);

    return queue;
}

PRIO_QUEUE_t *initPriorityQueue(){
    PRIO_QUEUE_t *queue = malloc(sizeof(PRIO_QUEUE_t));
    queue->high = initFIFOQueue();
    queue->medium = initFIFOQueue();
    queue->low = initFIFOQueue();

    return queue;
}

void initSchedulerQueues() {
    // Priority Queue (Ready)
    readyQueue = initPriorityQueue();

    // State Queues
    runningQueue = initFIFOQueue();
    blockedQueue = initFIFOQueue();
    finishedQueue = initFIFOQueue();

    // Join Pair Queue
    joinQueue = initFIFOQueue();
}

int initMainThread() {
    mainThreadTCB = malloc(sizeof(TCB_t));

    initSchedulerQueues();

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

TCB_t *getRunningThread(){
    TCB_t *thread = malloc(sizeof(TCB_t));
    FirstFila2(runningQueue);
    thread = GetAtIteratorFila2(runningQueue);

    return thread;
}

TCB_t *popPriorityThread(PRIO_QUEUE_t *queue){
    TCB_t *thread = malloc(sizeof(TCB_t));

    // Verifica se existe alguma thread de prioridade alta
    if(FirstFila2(queue->high) == 0){
        thread = GetAtIteratorFila2(queue->high);
        DeleteAtIteratorFila2(queue->high);
    } else if(FirstFila2(queue->medium) == 0){
        thread = GetAtIteratorFila2(queue->medium);
        DeleteAtIteratorFila2(queue->medium);
    } else if(FirstFila2(queue->low) == 0){
        thread = GetAtIteratorFila2(queue->low);
        DeleteAtIteratorFila2(queue->low);
    } else
        return NULL;

    return thread;
}

int scheduleNewThread() {
    TCB_t *thread = malloc(sizeof(TCB_t));
    thread = popPriorityThread(readyQueue);

    if(thread == NULL)
        return -1;

    runThread(thread);
    return 0;
}

int blockThread(){
    // Identifica a thread que está em execução
    TCB_t *thread = getRunningThread();
    int hasBeenBlocked = 0;

    if(thread == NULL)
        return -1;

    FirstFila2(runningQueue);
    DeleteAtIteratorFila2(runningQueue);
    // Salva o contexto de execução da thread
    getcontext(&(thread->context));

    if(hasBeenBlocked == 0){
        hasBeenBlocked = 1;
        thread->state = PROCST_BLOQ;
        AppendFila2(blockedQueue, thread);
        scheduleNewThread();
    }

    return 0;
}

int yield(){
    // Identifica a thread que está em execução
    TCB_t *thread = getRunningThread();
    int hasYielded = 0;

    if(thread == NULL)
        return -1;

    FirstFila2(runningQueue);
    DeleteAtIteratorFila2(runningQueue);
    // Salva o contexto de execução da thread
    getcontext(&(thread->context));

    if(hasYielded == 0){
        hasYielded = 1;
        thread->state = PROCST_APTO;
        insertReadyQueue(thread);
        // Chama o escalonador para executar outra thread
        scheduleNewThread();
    }

    return 0;
}

int unlockThread(int tid) {
    // Percorre a fila de threads bloqueadas
    TCB_t *thread = malloc(sizeof(TCB_t));
    FirstFila2(blockedQueue);
    do{
        thread = (TCB_t*) GetAtIteratorFila2(blockedQueue);
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
    TCB_t *thread = getRunningThread();

    // Remove a thread da fila "executando"
    FirstFila2(runningQueue);
    DeleteAtIteratorFila2(runningQueue);
    // Atualiza o estado da thread
    thread->state = PROCST_TERMINO;
    
    // Verifica se existe alguma thread esperando pela thread finalizada
    JOIN_PAIR_t *joinPair = malloc(sizeof(JOIN_PAIR_t));
    FirstFila2(joinQueue);
    do{
        joinPair = (JOIN_PAIR_t*) GetAtIteratorFila2(joinQueue);
        if(joinPair == NULL)
            break;
        if(joinPair->tid_running_thread == thread->tid){
            DeleteAtIteratorFila2(joinQueue);
            unlockThread(joinPair->tid_blocked_thread);
        }
    }while(NextFila2(joinQueue) == 0);

    AppendFila2(finishedQueue, thread);
    scheduleNewThread();
}

int setRunningThreadPriority(int prio){
    TCB_t *thread = getRunningThread();

    if(thread == NULL)
        return -1;

    thread->prio = prio;
    return 0;
}

TCB_t *findReadyThreadByTID(int tid){
    // Percorre a fila de prioridade alta
    TCB_t *thread = malloc(sizeof(TCB_t));
    FirstFila2(readyQueue->high);
    do{
        thread = (TCB_t*) GetAtIteratorFila2(readyQueue->high);
        if(thread == NULL)
            break;
        if(thread->tid == tid){
            // Se encontrar a thread, retorna um ponteiro para o seu TCB
            return thread;
        }
    } while(NextFila2(readyQueue->high) == 0);

    // Percorre a fila de prioridade média
    FirstFila2(readyQueue->medium);
    do{
        thread = (TCB_t*) GetAtIteratorFila2(readyQueue->medium);
        if(thread == NULL)
            break;
        if(thread->tid == tid){
            // Se encontrar a thread, retorna um ponteiro para o seu TCB
            return thread;
        }
    } while(NextFila2(readyQueue->medium) == 0);

    // Percorre a fila de prioridade baixa
    FirstFila2(readyQueue->low);
    do{
        thread = (TCB_t*) GetAtIteratorFila2(readyQueue->low);
        if(thread == NULL)
            break;
        if(thread->tid == tid){
            // Se encontrar a thread, retorna um ponteiro para o seu TCB
            return thread;
        }
    } while(NextFila2(readyQueue->low) == 0);

    // Se não encontrar a thread, retorna NULL
    return NULL;
}

int waitForThread(int tid){
    // Thread em execução
    TCB_t *thread = getRunningThread();

    // Verifica se a thread a ser bloqueada existe
    if(thread == NULL)
        return -1;

    // Verifica se a thread bloqueante existe
    if(findReadyThreadByTID(tid) == NULL)
        return -2;

    // Verifica se a thread bloqueante já bloqueia outra thread
    JOIN_PAIR_t *joinPair = malloc(sizeof(JOIN_PAIR_t));
    if(FirstFila2(joinQueue) == 0){
        do{
            joinPair = (JOIN_PAIR_t*) GetAtIteratorFila2(joinQueue);
            if(joinPair == NULL)
                break;
            if(joinPair->tid_running_thread == tid)
                return -3;
        }while(NextFila2(joinQueue) == 0);
    }

    joinPair->tid_blocked_thread = thread->tid;
    joinPair->tid_running_thread = tid;
    AppendFila2(joinQueue, joinPair);

    blockThread();
    return 0;
}