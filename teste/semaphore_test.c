#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

csem_t impressora;
int int_var;

void* func0(void *arg) {
	printf("[tid1]	======== Inicio da func0 ========\n");
	printf("[tid1]	Executando \n");
	printf("[tid1]  Liberando recurso do semaforo\n");
	printf("[tid1]  Liberando recurso do semaforo %d\n", csignal(&impressora));
	printf("[tid1]	Executando \n");
	printf("[tid1]  Requisitando recurso do semaforo\n");
	printf("[tid1]  Requisitando recurso do semaforo: %d\n", cwait(&impressora));
	printf("[tid1]	Executando \n");
	printf("[tid1]  Chamando cyield()\n");
	printf("[tid1]  Chamando cyield(). Ret: %d\n", cyield());
	printf("[tid1]  Executando\n");
	printf("[tid1]	Chamando csem_init com um csem_t*\n");
	printf("[tid1]  Chamando csem_init com um csem_t*: %d\n", csem_init(&impressora, 1));
	printf("[tid1]	Executando \n");
	printf("[tid1]  Liberando recurso do semaforo\n");
	printf("[tid1]  Liberando recurso do semaforo %d\n", csignal(&impressora));
	printf("[tid1]	Executando \n");
	printf("[tid1]	======== Fim da func0 ========\n");
}

void* func1(void *arg) {
	printf("[tid2]	======== Inicio da func1 ========\n");
	printf("[tid2]  Executando\n");
	printf("[tid2]  Liberando recurso do semaforo %d\n", csignal(&impressora));
	printf("[tid2]	Executando \n");
	printf("[tid2]  Liberando recurso do semaforo %d\n", csignal(&impressora));
	printf("[tid2]	Executando \n");
	printf("[tid2]  Liberando recurso do semaforo %d\n", csignal(&impressora));
	printf("[tid2]	Executando \n");
	printf("[tid2]  Requisitando recurso do semaforo\n");
	printf("[tid2]  Requisitando recurso do semaforo: %d\n", cwait(&impressora));
	printf("[tid2]  Executando\n");
	printf("[tid2]  Chamando cyield()\n");
	printf("[tid2]  Chamando cyield(). Ret: %d\n", cyield());
	printf("[tid2]  Executando\n");
	printf("[tid2]  Liberando recurso do semaforo %d\n", csignal(&impressora));
	printf("[tid2]	Executando \n");
	printf("[tid2]	======== Fim da func1 ========\n");
}


int main(int argc, char *argv[]) {
	int i = 0;
	printf("\n[main]	================ Inicio da main ================\n");
	printf("[main]	Executando \n");
	printf("[main]  Chamando csem_init com um int: %d\n", csem_init(int_var, 1));
	printf("[main]  Chamando csem_init com um int*: %d\n", csem_init(&int_var, 1));
	printf("[main]  Chamando csem_init com um csem_t*: %d\n", csem_init(&impressora, 1));

	printf("[main]	Criando thread func0 com prioridade 0. Ret: %d\n", ccreate(func0, (void *)&i, 0));
	printf("[main]	Criando thread func1 com prioridade 0. Ret: %d\n", ccreate(func1, (void *)&i, 0));
	
	printf("[main]	Chamando cjoin(tid2)\n");
	printf("[main]	Chamando cjoin(tid2) Ret: %d\n", cjoin(2));

	printf("[main]	================ Fim da main ================\n");
}

