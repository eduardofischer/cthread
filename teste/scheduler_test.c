
#include "../include/cthread.h"
#include <stdio.h>


void* func1(void *arg) {
	printf("[tid1]	======== Inicio da func1 ========\n");

	printf("[tid1]	Executando \n");
	printf("[tid1]	Imprimindo parametro: %d \n", *((int *)arg));
	
	printf("[tid1]	Chamando cyield()\n");
	printf("[tid1]	cyield return: %d\n", cyield());

	printf("[tid1]	Executando \n");

	printf("[tid1]	======== Fim da func1 ========\n");
}

void* func2(void *arg) {
	printf("[tid2]	======== Inicio da func2 ========\n");

	printf("[tid2]	Executando \n");
	printf("[tid2]	Imprimindo parametro: %d \n", *((int *)arg));

	printf("[tid2]	Chamando cyield()\n");
	printf("[tid2]	cyield return: %d\n", cyield());

	printf("[tid2]	Executando \n");

	printf("[tid2]	Chamando cyield()\n");
	printf("[tid2]	cyield return: %d\n", cyield());

	printf("[tid2]	======== Fim da func2 ========\n");
}

void* func3(void *arg) {
	printf("[tid3]	======== Inicio da func3 ========\n");

	printf("[tid3]	Executando \n");
	printf("[tid3]	Imprimindo parametro: %d \n", *((int *)arg));

	printf("[tid3]	Chamando cyield()\n");
	printf("[tid3]	cyield return: %d\n", cyield());

	printf("[tid3]	Executando \n");

	printf("[tid3]	======== Fim da func3 ========\n");	
}

void* func4(void *arg) {
	printf("[tid4]	======== Inicio da func4 ========\n");

	printf("[tid4]	Executando \n");
	printf("[tid4]	Imprimindo parametro: %d \n", *((int *)arg));

	printf("[tid4]	======== Fim da func4 ========\n");	
}

int main(int argc, char *argv[]) {
	int i = 99;

	int tid1, tid2, tid3, tid4;

	printf("\n[main]	================ Inicio da main ================\n");

	printf("[main]	Executando \n");

	printf("[main]	Criando thread: tid1 c/ prioridade 2\n");
	tid1 = ccreate(func1, (void *)&i, 2);
	printf("[main]	ccreate func1 return: %d\n", tid1);

	printf("[main]	Criando thread: tid2 c/ prioridade 0\n");
	tid2 = ccreate(func2, (void *)&i, 0);
	printf("[main]	ccreate func2 return: %d\n", tid2);

	printf("[main]	Criando thread: tid3 c/ prioridade 1\n");
	tid3 = ccreate(func3, (void *)&i, 1);
	printf("[main]	ccreate func3 return: %d\n", tid3);

	printf("[main]	Executando \n");

	printf("[main]	Criando thread: tid4 c/ prioridade 0\n");
	tid4 = ccreate(func4, (void *)&i, 0);
	printf("[main]	ccreate func4 return: %d\n", tid4);

	printf("[main]	Chamando cjoin(tid1)\n");
	printf("[main]	cjoin tid1 return: %d\n", cjoin(tid1));

	printf("[main]	Executando \n");

	printf("[main]	Chamando cjoin(tid2)\n");
	printf("[main]	cjoin tid2 return: %d\n", cjoin(tid2));

	printf("[main]	Executando \n");

	printf("[main]	================ Fim da main ================\n\n");
}

