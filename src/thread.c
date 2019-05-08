#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/thread.h"
#include "../include/scheduler.h"

int tid = 0;

void createContext(ucontext_t* newContext, void* (*start)(void*), void* arg){
    // Criação do contexto que será chamado ao fim da execução da nova thread, que devolve a execução ao escalonador
    ucontext_t* schedulerContext = malloc(sizeof(ucontext_t));
    // O contexto atual é capturado e serve de base para a criação do novo contexto
    getcontext(schedulerContext);
    // Determina o espaço de pilha
    schedulerContext->uc_stack.ss_sp = malloc(STACK_SIZE);
    schedulerContext->uc_stack.ss_size = STACK_SIZE;
    schedulerContext->uc_stack.ss_flags = 0;
    makecontext(schedulerContext, (void*)killThread, 0);
    // Criação do novo contexto
    getcontext(newContext);
    newContext->uc_stack.ss_sp = malloc(STACK_SIZE);
    newContext->uc_stack.ss_size = STACK_SIZE;
    newContext->uc_stack.ss_flags = 0;
    newContext->uc_link = schedulerContext;
    makecontext(newContext, (void*)start, 1, arg);
}

TCB_t* createThread(ucontext_t context, int priority){
    TCB_t *thread = malloc(sizeof(TCB_t));
    *thread = (TCB_t) {
        .tid = tid,
        .state = PROCST_NOVO,
        .prio = priority,
        .context = context
    };
    tid++;
    return thread;
}