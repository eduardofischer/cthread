
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/thread.h"
#include "../include/scheduler.h"
#include "../include/semaphore.h"

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
// * -2: Erro ao criar a thread main
int csetprio(int tid, int prio) {
	// Inicializa a thread main caso ainda não tenha sido inicializada
	if(!hasInitMainThread){
		if(initMainThread() == 0)
			hasInitMainThread = 1;
		else
			return -2;
	}
	if(prio == HIGH_PRIORITY || prio == MEDIUM_PRIORITY || prio == LOW_PRIORITY){
		return setRunningThreadPriority(prio);
	} else
		return -1;
}

// Função: A thread em execução cede a CPU voluntariamente
// Retorno:
// *  0: Sucesso!
// * -1: Erro
int cyield(void) {
	// Inicializa a thread main caso ainda não tenha sido inicializada
	if(!hasInitMainThread){
		if(initMainThread() == 0)
			hasInitMainThread = 1;
		else
			return -2;
	}
	return yield();
}

// Função: Bloqueia a thread em execução até o termino da execução de outra thread
// Retorno:
// *  0: Sucesso!
// * -1: Erro
// * -2: Erro ao criar a thread main
// * -3: Erro: a thread bloqueante não existe
// * -4: Erro: a thread bloqueante já bloqueia outra thread
int cjoin(int tid) {
	// Inicializa a thread main caso ainda não tenha sido inicializada
	if(!hasInitMainThread){
		if(initMainThread() == 0)
			hasInitMainThread = 1;
		else
			return -2;
	}
	return waitForThread(tid);
}

// Função: Inicializa um uma estrutura do tipo semáforo
// Retorno:
// *  0: Sucesso!
// * -1: Erro
// * -2: Erro ao criar a thread main
// * -3: Semáforo inválido
int csem_init(csem_t *sem, int count) {
	// Inicializa a thread main caso ainda não tenha sido inicializada
	if(!hasInitMainThread){
		if(initMainThread() == 0)
			hasInitMainThread = 1;
		else
			return -2;
	}

	if(sem == NULL)
		return -3;

	sem->count = count;
	sem->fila = initFIFOQueue();

    return 0;
}

// Função: Solicita o uso do recurso protegido por semáforo, e gerencia uma fila caso o recurso esteja ocupado
// Retorno:
// *  0: Sucesso!
// * -1: Erro
int cwait(csem_t *sem) {
	if(sem == NULL || sem->fila == NULL)
		return -1;
	
	(sem->count)--;

	// Se o recurso não estiver disponível, coloca a thread na fila de espera
	if(sem->count < 0)
		return waitForResource(sem);

	return 0;
}

// Função: Libera um recurso protegido por semáforo
// Retorno:
// *  0: Sucesso!
// * -1: Erro
int csignal(csem_t *sem) {
	if(sem == NULL || sem->fila == NULL)
		return -1;
	
	(sem->count)++;

	// Se o recurso não estiver disponível, coloca a thread na fila de espera
	if(sem->count < 1)
		return unlockSemThread(sem);

	return 0;
}

// Função: Copia para uma área de memória os nomes e números de cartão dos componentes do grupo
// *  0: Sucesso!
int cidentify (char *name, int size) {
	strncpy (name, "Eduardo Spitzer Fischer - 00290399\nMaria Flávia Borrajo Tondo - 00278892\nRodrigo Paranhos Bastos - 00261162", size);
	return 0;
}


