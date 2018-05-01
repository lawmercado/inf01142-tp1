#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdlib.h>

void* func0(void *arg)
{
    cwait((csem_t *)arg);

    printf("ID0: 1: Eu sou a thread ID0 imprimindo count do semáforo: %d\n", ((csem_t *)arg)->count);

    cyield();

    csignal((csem_t *)arg);

    printf("ID0: 2: Eu sou a thread ID0 imprimindo count do semáforo: %d\n", ((csem_t *)arg)->count);

    return 0;
}

void* func1(void *arg)
{
    printf("ID1: 1: Eu sou a thread ID1 imprimindo count do semáforo: %d\n", ((csem_t *)arg)->count);

    cwait((csem_t *)arg);

    printf("ID1: 2: Eu sou a thread ID1 imprimindo count do semáforo: %d\n", ((csem_t *)arg)->count);

    return 0;
}

void* func2(void *arg)
{
    printf("ID2: 1: Eu sou a thread ID2 imprimindo count do semáforo: %d\n", ((csem_t *)arg)->count);

    cwait((csem_t *)arg);

    printf("ID2: 2: Eu sou a thread ID2 imprimindo count do semáforo: %d\n", ((csem_t *)arg)->count);

    return 0;
}

int main (int argc, char *argv[])
{
    int id0, id1, id2;
    csem_t semaforo1;

    if(csem_init(&semaforo1, 1) == 0)
    {
        id0 = ccreate(func0, (void*)&semaforo1, 0);
        id1 = ccreate(func1, (void*)&semaforo1, 0);
        id2 = ccreate(func2, (void*)&semaforo1, 0);

        printf("MAIN: Eu sou a main após a criação de ID0, ID1\n");
        cjoin(id2);

        printf("MAIN: Eu sou a main voltando para terminar o programa \n");
    }
    else
    {
        printf("MAIN: Erro ao criar o semáforo!\n");
    }

    return 0;
}
