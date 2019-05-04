
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/thread.h"
#include "../include/scheduler.h"

// Flag de inicialização da thread main
int hasInitMainThread = 0;

// Função ccreate: Cria uma nova thread
// Retorno:
// *  0: Sucesso!
// * -1: Erro ao criar a thread
// * -2: Erro ao criar a thread main
// * -3: Função inválida
int ccreate (void* (*start)(void*), void *arg, int prio) {
	// Inicializa a thread main caso ainda não tenha sido inicializada
	if(!hasInitMainThread){
		if(initMainThread() == 0)
			hasInitMainThread = 1;
		else
			return -2;
	}
	// Verifica a função a ser executada
	if(start == NULL)
		return -3;

	// Criação do contexto
	ucontext_t *newContext = malloc(sizeof(ucontext_t));
	createContext(newContext, start, arg);

	// Criação do TCB
	TCB_t *newThread = malloc(sizeof(TCB_t));
	newThread = createThread(*newContext, prio);
	insertReadyQueue(newThread);
	return newThread->tid;
}
 
// Função: Altera a prioridade da própria thread em execução, sem preempção
// Retorno:
// *  0: Sucesso!
// * -1: Erro
int csetprio(int tid, int prio) {
	if(prio == HIGH_PRIORITY || prio == MEDIUM_PRIORITY || prio == LOW_PRIORITY){
		if(setRunningThreadPriority(prio) == 0)
			return 0;
		else
			return -1;
	} else
		return -1;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {
	return -1;
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
	strncpy (name, "Sergio Cechin - 2017/1 - Teste de compilacao.", size);
	return 0;
}


