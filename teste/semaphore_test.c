
/*
 *	Programa de exemplo de uso da biblioteca cthread
 *
 *	Versão 1.0 - 14/04/2016
 *
 *	Sistemas Operacionais I - www.inf.ufrgs.br
 *
 */

#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

csem_t impressora;


void* func0(void *arg) {
	printf("Eu sou a thread ID0 usando o semáforo\n");
	printf("Eu sou a thread ID0. Ret cwait: %d\n", cwait(&impressora));
	printf("Eu sou a thread ID0 chamando cyield()\n");
	printf("Eu sou a thread ID0. Ret cyield: %d\n", cyield());
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	printf("Eu sou a thread ID0 liberando o semáforo\n");
	printf("Eu sou a thread ID0. Ret csignal: %d\n", csignal(&impressora));
	printf("Eu sou a thread ID0\n");
	printf("Eu sou a thread ID0\n");
	printf("Eu sou a thread ID0\n");
	printf("Eu sou a thread ID0\n");
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1 pedindo o semáforo\n");
	printf("Eu sou a thread ID1. Ret cwait: %d\n", cwait(&impressora));
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1\n");
	printf("Eu sou a thread ID1 mudando minha prioridade para MEDIUM. Ret: %d\n", csetprio(0, 1));
	
}


int main(int argc, char *argv[]) {

	int	id0, id1;
	int	join0, join1;

	int i = 5;

	printf("Eu sou a main antes da criação de ID0 e ID1\n");

	printf("sem0: %d\n\n", csem_init(&impressora, 1));

	id0 = ccreate(func0, (void *)&i, 0);
	id1 = ccreate(func1, (void *)&i, 0);

	printf("id0: %d\nid1: %d\n\n", id0, id1);

	printf("Eu sou a main após a criação de ID0 e ID1\n\n");

	join0 = cjoin(id0);
	join1 = cjoin(id1);

	printf("\njoin0: %d\njoin1: %d\n\n", join0, join1);

	printf("Eu sou a main voltando para terminar o programa\n\n");
}

