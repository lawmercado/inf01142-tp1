#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdlib.h>

void* func0(void *arg)
{
    printf("ID0: 1: Eu sou a thread ID0 imprimindo meu argumento (%d) pela primeira vez\n", *((int *)arg));

    cyield();

    printf("ID0: 2: Eu sou a thread ID0 imprimindo meu argumento (%d) pela segunda vez\n", *((int *)arg));

    return 0;
}

void* func1(void *arg)
{
    printf("ID1: 1: Eu sou a thread ID1 imprimindo meu argumento (%d) pela primeira vez\n", *((int *)arg));

    cyield();

    printf("ID1: 2: Eu sou a thread ID1 imprimindo meu argumento (%d) pela segunda vez\n", *((int *)arg));

    return 0;
}

void* func2(void *arg)
{
    printf("ID2: 1: Eu sou a thread ID2 imprimindo meu argumento (%d) pela primeira vez\n", *((int *)arg));

    cyield();

    printf("ID2: 2: Eu sou a thread ID2 imprimindo meu argumento (%d) pela segunda vez\n", *((int *)arg));

    return 0;
}

int main (int argc, char *argv[])
{
    char nomes[200];
    int id0, id1, id2;
    int valor1 = 10, valor2 = 11, valor3 = 13;

    cidentify(nomes, 200);
    printf("-GRUPO:\n");
    printf("%s", nomes);

    printf("\n-OBJETIVO DO TESTE: Testar as funcionalidades básicas da biblioteca cthread. Espera se a impressão alternada de strings por parte da thread ID1 e ID2, enquanto ID0 está suspensa. Em seguida, após resumir ID0, espera-se que as duas impressões da mesma sejam realizadas.\n");

    printf("\n-EXECUÇÃO DO TESTE\n");

    printf("MAIN: Eu sou a main ANTES da criação de ID0, ID1 e ID2\n");

    printf("TESTE: Suspend numa thread inexistente: %d, valor esperado -1\n", csuspend(5));
    printf("TESTE: Resume numa thread inexistente: %d, valor esperado -1\n", cresume(5));
    printf("TESTE: Suspend em si mesmo: %d, valor esperado -1\n", csuspend(0));
    printf("TESTE: Resume em si mesmo: %d, valor esperado -1\n", cresume(0));
    printf("TESTE: Yield sem nenhuma outra thread: %d, valor esperado -1\n", cyield());

    id0 = ccreate(func0, (void*)&valor1, 0);
    id1 = ccreate(func1, (void*)&valor2, 0);
    id2 = ccreate(func2, (void*)&valor3, 0);

    printf("MAIN: Eu sou a main APÓS a criação de ID0, ID1 e ID2\n");

    printf("TESTE: Suspend numa thread existente (ID0): %d, valor esperado 0\n", csuspend(id0));
    printf("TESTE: Join numa thread existente (ID2): %d, valor esperado 0\n", cjoin(id2));
    printf("TESTE: Resume numa thread existente (ID0): %d, valor esperado 0\n", cresume(id0));

    cjoin(id0);

    printf("TESTE: Resume numa thread finalizada: %d, valor esperado -1\n", cresume(id0));
    printf("TESTE: Suspend numa thread finalizada: %d, valor esperado -1\n", csuspend(id0));
    printf("TESTE: Join numa thread finalizada: %d, valor esperado -1\n", cjoin(id0));
    printf("TESTE: Join numa thread inexistente: %d, valor esperado -1\n", cjoin(5));
    printf("TESTE: Join em si mesmo: %d, valor esperado -1\n", cjoin(0));
    printf("TESTE: Yield sem nenhuma outra thread %d, valor esperado 0\n", cyield());

    printf("MAIN: Eu sou a main voltando para terminar o programa \n");

    return 0;
}
