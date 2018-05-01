#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdlib.h>

void* func0(void *arg)
{
    cwait((csem_t *)arg);

    printf("TESTE: ID0: 1: Valor do contador do semaforo APÓS ID0 solicitar o recurso: %d, valor esperado 0\n", ((csem_t *)arg)->count);

    cyield();

    csignal((csem_t *)arg);

    printf("TESTE: ID0: 2: Valor do contador do semaforo APÓS ID0 liberar o recurso: %d, valor esperado -1\n", ((csem_t *)arg)->count);

    return 0;
}

void* func1(void *arg)
{
    printf("TESTE: ID1: 1: Valor do contador do semaforo ANTES de ID1 solicitar o recurso: %d, valor esperado 0\n", ((csem_t *)arg)->count);
    cwait((csem_t *)arg);

    printf("TESTE: ID1: 2: Valor do contador do semaforo APÓS ID1 solicitar o recurso: %d, valor esperado -1\n", ((csem_t *)arg)->count);

    csignal((csem_t *)arg);

    printf("TESTE: ID1: 3: Valor do contador do semaforo APÓS ID1 liberar o recurso: %d, valor esperado 0\n", ((csem_t *)arg)->count);

    return 0;
}

void* func2(void *arg)
{
    printf("TESTE: ID2: 1: Valor do contador do semaforo ANTES de ID2 solicitar o recurso: %d, valor esperado -1\n", ((csem_t *)arg)->count);

    cwait((csem_t *)arg);

    printf("TESTE: ID2: 2: Valor do contador do semaforo APÓS ID2 solicitar o recurso: %d, valor esperado 0\n", ((csem_t *)arg)->count);

    csignal((csem_t *)arg);

    printf("TESTE: ID2: 3: Valor do contador do semaforo APÓS ID2 liberar o recurso: %d, valor esperado 1\n", ((csem_t *)arg)->count);

    return 0;
}

int main (int argc, char *argv[])
{
    char nomes[200];
    int id0, id1, id2;
    csem_t semaforo1;

    cidentify(nomes, 200);
    printf("-GRUPO:\n");
    printf("%s", nomes);

    printf("\n-OBJETIVO DO TESTE: Testar as funcionalidades relacionadas aos semáforos. Passos da execução do teste:\n");
    printf("ID0 solicita o recurso e chama cyield\n");
    printf("ID1 solicita o recurso e é bloqueada\n");
    printf("ID2 solicita o recurso e é bloqueada\n");
    printf("ID0 libera o recurso e termina a execução\n");
    printf("ID1 libera o recurso e termina a execução\n");
    printf("ID2 libera o recurso e termina a execução\n");

    printf("\n-EXECUÇÃO DO TESTE\n");

    if(csem_init(&semaforo1, 1) == 0)
    {
        id0 = ccreate(func0, (void*)&semaforo1, 0);
        id1 = ccreate(func1, (void*)&semaforo1, 0);
        id2 = ccreate(func2, (void*)&semaforo1, 0);

        printf("MAIN: Eu sou a main após a criação de ID0, ID1 e ID2\n");
        cjoin(id2);

        printf("MAIN: Eu sou a main voltando para terminar o programa \n");
    }
    else
    {
        printf("MAIN: Erro ao criar o semáforo!\n");
    }

    return 0;
}
