/*
 * cdata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * VERSÃO: 29/03/2019
 *
 */
#ifndef __cdata__
#define __cdata__
#include "ucontext.h"

/* Estados das threads
*/
#define PROCST_NOVO -1		/* Processo no momento de sua criação */
#define	PROCST_APTO	0		/* Processo em estado apto a ser escalonado */
#define	PROCST_EXEC	1		/* Processo em estado de execução */
#define	PROCST_BLOQ	2		/* Processo em estado bloqueado */
#define	PROCST_TERMINO	3	/* Processo em estado de terminado */

// Thread Control Block
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra (CONFORME DEFINES ANTERIORES)
	int 		prio;		// Prioridade associada a thread
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
} TCB_t; 

// Filas de prioridades
typedef struct s_PRIO_QUEUE {
	FILA2 *high;
	FILA2 *medium;
	FILA2 *low;
} PRIO_QUEUE_t;

// Join parent-child pair
typedef struct s_JOIN_PAIR {
	int tid_blocked_thread;
	int tid_running_thread;
} JOIN_PAIR_t;


#endif
