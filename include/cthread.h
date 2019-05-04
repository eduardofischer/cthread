/*
 * cthread.h: arquivo de inclus�o com os prot�tipos das fun��es a serem
 *            implementadas na realiza��o do trabalho.
 *
 * N�O MODIFIQUE ESTE ARQUIVO.
 *
 * VERS�O: 29/03/2019
 *
 */
#ifndef __cthread__
#define __cthread__

#include "support.h"
#include "cdata.h"

typedef struct s_sem {
	int	count;	/* indica se recurso est� ocupado ou n�o (livre > 0, ocupado = 0) */
	PRIO_QUEUE_t	*fila; 	/* ponteiro para uma fila de threads bloqueadas no sem�foro */
} csem_t;

/******************************************************************************
Par�metros:
	start:	ponteiro para a fun��o que a thread executar�.
	arg:	um par�metro que pode ser passado para a thread na sua cria��o.
	prio:	N�O utilizado neste semestre, deve ser sempre zero.
Retorno:
	Se correto => Valor positivo, que representa o identificador da thread criada
	Se erro	   => Valor negativo.
******************************************************************************/
int ccreate (void* (*start)(void*), void *arg, int prio);

/******************************************************************************
Par�metros:
	Sem par�metros
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int cyield(void);

/******************************************************************************
Par�metros:
	tid: identificador da thread cuja prioridade ser� alterada (deixar sempre esse campo como NULL em 2018/02)
	prio: nova prioridade da thread.
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int csetprio(int tid, int prio);

/******************************************************************************
Par�metros:
	tid:	identificador da thread cujo t�rmino est� sendo aguardado.
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int cjoin(int tid);

/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo csem_t. Aponta para uma estrutura de dados que representa a vari�vel sem�foro.
	count: valor a ser usado na inicializa��o do sem�foro. Representa a quantidade de recursos controlados pelo sem�foro.
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int csem_init(csem_t *sem, int count);

/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo sem�foro.
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int cwait(csem_t *sem);

/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo sem�foro.
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int csignal(csem_t *sem);

/******************************************************************************
Par�metros:
	name:	ponteiro para uma �rea de mem�ria onde deve ser escrito um string que cont�m os nomes dos componentes do grupo e seus n�meros de cart�o.
		Deve ser uma linha por componente.
	size:	quantidade m�xima de caracteres que podem ser copiados para o string de identifica��o dos componentes do grupo.
Retorno:
	Quando executada corretamente: retorna 0 (zero)
	Caso contr�rio, retorna um valor negativo.
******************************************************************************/
int cidentify (char *name, int size);


#endif

