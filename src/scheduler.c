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


PRIO_QUEUE_t *initPriorityQueue(){
    PRIO_QUEUE_t *queue = malloc(sizeof(PRIO_QUEUE_t));
    queue->high = malloc(sizeof(FILA2*));
    CreateFila2(queue->high);
    queue->medium = malloc(sizeof(FILA2*));
    CreateFila2(queue->medium);
    queue->low = malloc(sizeof(FILA2*));
    CreateFila2(queue->low);

    return queue;
}

int initQueues() {
    int retStatus = 0;

    // Priority Queue (Ready)
    readyQueue = initPriorityQueue();

    // State Queues
    runningQueue = malloc(sizeof(FILA2*));
    retStatus -= CreateFila2(runningQueue);
    blockedQueue = malloc(sizeof(FILA2*));
    retStatus -= CreateFila2(blockedQueue);
    finishedQueue = malloc(sizeof(FILA2*));
    retStatus -= CreateFila2(finishedQueue);

    // Join Pair Queue
    joinQueue = malloc(sizeof(FILA2*));
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

int insertPriorityQueue(PRIO_QUEUE_t *queue, TCB_t *thread){
    thread->state = PROCST_APTO;

    switch(thread->prio) {
        case HIGH_PRIORITY:
            AppendFila2(queue->high, thread);
            break;
        case MEDIUM_PRIORITY:
            AppendFila2(queue->medium, thread);
            break;
        case LOW_PRIORITY:
            AppendFila2(queue->low, thread);
            break;
        default:
            return -1;
    }

    return 0;
}

int insertReadyQueue(TCB_t *thread){
    return insertPriorityQueue(readyQueue, thread);
}

void runThread(TCB_t *thread) {
    thread->state = PROCST_EXEC;
    AppendFila2(runningQueue, thread);
    setcontext(&(thread->context));
}

TCB_t *popPriorityThread(PRIO_QUEUE_t *queue){
    TCB_t *thread = malloc(sizeof(TCB_t));

    // Verifica se existe alguma thread de prioridade alta
    if(FirstFila2(queue->high) == 0){
        thread = GetAtAntIteratorFila2(queue->high);
        DeleteAtIteratorFila2(queue->high);
    } else if(FirstFila2(queue->medium) == 0){
        thread = GetAtAntIteratorFila2(queue->medium);
        DeleteAtIteratorFila2(queue->medium);
    } else if(FirstFila2(queue->low) == 0){
        thread = GetAtAntIteratorFila2(queue->low);
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
    TCB_t *thread = GetAtAntIteratorFila2(runningQueue);

    if(thread == NULL)
        return -1;

    DeleteAtIteratorFila2(runningQueue);
    getcontext(&(thread->context));
    thread->state = PROCST_BLOQ;
    AppendFila2(blockedQueue, thread);
    scheduleNewThread();
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
        if(joinPair->tid_running_thread == thread->tid){
            DeleteAtIteratorFila2(joinQueue);
            unlockThread(joinPair->tid_blocked_thread);
        }
    }while(NextFila2(joinQueue) == 0);

    AppendFila2(finishedQueue, thread);
    scheduleNewThread();
}

int setRunningThreadPriority(int prio){
    TCB_t *thread = GetAtAntIteratorFila2(runningQueue);

    if(thread == NULL)
        return -1;

    thread->prio = prio;
    return 0;
}

int yield(){
    TCB_t *thread = GetAtAntIteratorFila2(runningQueue);

    if(thread == NULL)
        return -1;

    // Salva o contexto de execução da thread
    getcontext(&(thread->context));

    // Troca a fila da thread (executando -> apto)
    DeleteAtIteratorFila2(runningQueue);
    insertReadyQueue(thread);

    // Chama o escalonador para executar outra thread
    return scheduleNewThread();
}

TCB_t *findReadyThreadByTID(int tid){
    // Percorre a fila de prioridade alta
    TCB_t *thread;
    FirstFila2(readyQueue->high);
    do{
        thread = (TCB_t*) GetAtAntIteratorFila2(readyQueue->high);
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
        thread = (TCB_t*) GetAtAntIteratorFila2(readyQueue->medium);
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
        thread = (TCB_t*) GetAtAntIteratorFila2(readyQueue->low);
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
    TCB_t *blockedThread = GetAtAntIteratorFila2(runningQueue);

    // Verifica se a thread a ser bloqueada existe
    if(blockedThread == NULL)
        return -1;

    // Verifica se a thread bloqueante existe
    if(findReadyThreadByTID(tid) == NULL)
        return -1;

    // Verifica se a thread bloqueante já bloqueia outra thread
    JOIN_PAIR_t *joinPair;
    if(FirstFila2(joinQueue) == 0){
        do{
            joinPair = (JOIN_PAIR_t*) GetAtAntIteratorFila2(joinQueue);
            if(joinPair == NULL)
                break;
            if(joinPair->tid_running_thread == tid)
                return -1;
        }while(NextFila2(joinQueue) == 0);
    }

    joinPair->tid_blocked_thread = blockedThread->tid;
    joinPair->tid_running_thread = tid;
    AppendFila2(joinQueue, joinPair);

    blockThread();
    return 0;
}

int waitForResource(csem_t *sem){
    // Thread em execução
    TCB_t *blockedThread = GetAtAntIteratorFila2(runningQueue);

    // Verifica se a thread a ser bloqueada existe
    if(blockedThread == NULL)
        return -1;

    // Verifica se o semáforo existe
    if(sem == NULL || sem->fila == NULL)
        return -1;
 
    // Adiciona à fila do semáforo
    insertPriorityQueue(sem->fila, blockedThread);

    blockThread();
    return 0;
}

int unlockSemThread(csem_t *sem){
    TCB_t *thread = malloc(sizeof(TCB_t));
    thread = popPriorityThread(sem->fila);
    return unlockThread(thread->tid);
}



