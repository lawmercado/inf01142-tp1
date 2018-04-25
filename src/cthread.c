#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 4728552

#define NUM_FILAS 5

#define IDX_APTOS 0
#define IDX_FINALIZADOS 1
#define IDX_BLOQUEADOS 2
#define IDX_BLOQUEADOSSUSP 3
#define IDX_APTOSSUSP 4

/******************************************************************************
Filas gerenciadas pela api, em cada índice uma fila, conforme definição 'IDX_*'.
******************************************************************************/
FILA2 g_filas[NUM_FILAS];

/******************************************************************************
Thread em execução no momento.
******************************************************************************/
TCB_t *g_emExecucao;

/******************************************************************************
Ponteiro para a thread main.
******************************************************************************/
TCB_t *g_threadMain;

/******************************************************************************
Contexto da thread main, para onde todas as demais threads voltam.
'Home is where you main is'.
******************************************************************************/
ucontext_t *g_contextMain;

/******************************************************************************
Função responsável por inicializar as varíaveis/filas utilizadas pela api

Parâmetros:
	Sem parâmetros
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cinit ()
{
    int i = 0;
    TCB_t *tMain;

    // Inicializa as filas
    for(i = 0; i < NUM_FILAS; i++)
    {
        if(CreateFila2(&g_filas[i]) != 0)
        {
            return -1;
        }
    }

    // Cria a entrada para a main e coloca a mesma como executando
    tMain = (TCB_t *) malloc(sizeof(TCB_t));
    tMain->tid = 0;
	tMain->state = PROCST_EXEC;
    tMain->joined_tid = NO_JOINED_TID;
    getcontext(&(tMain->context));

    g_contextMain = &(tMain->context);
    g_threadMain = tMain;
    g_emExecucao = tMain;

    return 0;
}

/******************************************************************************
Função para visualizar o estado geral da implementação

Parâmetros:
	Sem parâmetros
Retorno:
	Sem retorno
******************************************************************************/
void mostrarEstado() {
    printf("\n==== ESTADO ATUAL ====\n");
    printf("\n==== EM EXECUÇÃO ====\n");
    printf("PID %d; STATE %d; ASSOC %d\n", g_emExecucao->tid, g_emExecucao->state, g_emExecucao->joined_tid);

    printf("\n==== FILA DE APTOS ====\n");

    TCB_t *conteudo = NULL;

    if(FirstFila2(&g_filas[IDX_APTOS]) == 0)
    {
        while(GetAtIteratorFila2(&g_filas[IDX_APTOS]) != NULL)
        {
            conteudo = (TCB_t *) GetAtIteratorFila2(&g_filas[IDX_APTOS]);
            printf("PID %d; STATE %d; ASSOC %d\n", conteudo->tid, conteudo->state, conteudo->joined_tid);

            NextFila2(&g_filas[IDX_APTOS]);

        }
    }

    printf("\n");

    printf("\n==== FILA DE BLOQUEADOS ====\n");

    if(FirstFila2(&g_filas[IDX_BLOQUEADOS]) == 0)
    {
        while(GetAtIteratorFila2(&g_filas[IDX_BLOQUEADOS]) != NULL)
        {
            conteudo = (TCB_t *) GetAtIteratorFila2(&g_filas[IDX_BLOQUEADOS]);
            printf("PID %d; STATE %d; ASSOC %d\n", conteudo->tid, conteudo->state, conteudo->joined_tid);

            NextFila2(&g_filas[IDX_BLOQUEADOS]);

        }
    }

    printf("\n");
}

/******************************************************************************
Parâmetros:
	name:	ponteiro para uma área de memória onde deve ser escrito um string que contém os nomes dos componentes do grupo e seus números de cartão.
            Deve ser uma linha por componente.
	size:	quantidade máxima de caracteres que podem ser copiados para o string de identificação dos componentes do grupo.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cidentify (char *name, int size)
{
    sprintf(name, "Luís Augusto Weber Mercado - 265041\nMatheus Tavares Frigo - 262521\nNicholas de Aquino Lau - 268618\n");

    return 0;
}

/******************************************************************************
Parâmetros:
	start:	ponteiro para a função que a thread executará.
	arg:	um parâmetro que pode ser passado para a thread na sua criação.
	prio:	NÃO utilizado neste semestre, deve ser sempre zero.
Retorno:
	Se correto => Valor positivo, que representa o identificador da thread criada
	Se erro	   => Valor negativo.
******************************************************************************/
int ccreate (void* (*start)(void*), void *arg, int prio)
{
    int status;
    int tid;
    TCB_t *t = NULL;

    // Se fila de aptos estiver vazia
    if( LastFila2(&g_filas[IDX_APTOS]) != 0 )
    {
        status = cinit();

        if(status == -1)
        {
            return -1;
        }
    }

    tid = Random2();
    t = (TCB_t *) malloc(sizeof(TCB_t));

    t->tid = tid;
    t->state = PROCST_APTO;
    t->joined_tid = NO_JOINED_TID;

    getcontext(&(t->context));

    t->context.uc_link = g_contextMain;
    if ((t->context.uc_stack.ss_sp = (char *) malloc(STACK_SIZE)) != NULL)
    {
        t->context.uc_stack.ss_size = STACK_SIZE;
        t->context.uc_stack.ss_flags = 0;
        makecontext(&(t->context), (void (*) (void))start, 1, arg);

        AppendFila2(&g_filas[IDX_APTOS], (void *)t);

        return tid;
    }

    return -1;
}

/******************************************************************************
Dispatcher reponsável por pegar uma thread na fila de aptos e colocar a mesma
para executá-la.

Parâmetros:
	Sem parâmetros
Retorno:
	Sem retorno.
******************************************************************************/
void __dispatch()
{
    TCB_t *candidata = NULL;
    TCB_t *yielded = g_emExecucao;

    // Pega a primeira thread da fila de aptos
    FirstFila2(&g_filas[IDX_APTOS]);

    candidata = (TCB_t *) GetAtIteratorFila2(&g_filas[IDX_APTOS]);
    candidata->state = PROCST_EXEC;
    g_emExecucao = candidata;
    DeleteAtIteratorFila2(&g_filas[IDX_APTOS]);

    swapcontext(&(yielded->context), &(candidata->context));
}

/******************************************************************************
Ações a serem realizadas após a execução da thread indicada pelo dispatcher
Parâmetros:
	Sem parâmetros
Retorno:
	Sem retorno.
******************************************************************************/
void __setupContextoPosDispatch()
{
    g_emExecucao = g_threadMain;
    g_emExecucao->state = PROCST_EXEC;
}

/******************************************************************************
Parâmetros:
	Sem parâmetros
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cyield(void)
{
    g_emExecucao->state = PROCST_APTO;

    AppendFila2(&g_filas[IDX_APTOS], (void *)g_emExecucao);

    __dispatch();

    __setupContextoPosDispatch();

    return 0;
}

/******************************************************************************
Procura numa dada fila, o tid-a-ser-associado-com. Se encontrar, realiza a assoc.

Parâmetros:
	fila:	ponteiro para a fila em que deve ser pesquisado o tid
	tid:	tid da thread que solicitou o join
    associarComTid: tid da thread a ser associada
Retorno:
	Se correto (encontrar tid-a-ser-associado-com) => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int __associarJoin(FILA2 *fila, int tid, int associarComTid)
{
    TCB_t *conteudo = NULL;

    if(FirstFila2(fila) == 0)
    {
        while(GetAtIteratorFila2(fila) != NULL)
        {
            conteudo = (TCB_t *) GetAtIteratorFila2(fila);

            if(conteudo->tid == associarComTid)
            {
                if(conteudo->joined_tid == NO_JOINED_TID)
                {
                    conteudo->joined_tid = tid;

                    return 0;
                }
            }

            NextFila2(fila);
        }
    }

    return -1;
}

/******************************************************************************
Remove de uma dada fila o tid informado

Parâmetros:
    tid:	tid da thread a ser removida
    fila:	ponteiro para a fila em que deve ser removido a thread com o tid
Retorno:
	Se correto (encontrar tid-a-ser-associado-com) => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int __removerDaFilPorTid(int tid, FILA2 *fila)
{
    int removed = -1;
    TCB_t *conteudo = NULL;

    if(FirstFila2(fila) == 0)
    {
        while(GetAtIteratorFila2(fila) != NULL && removed == -1)
        {
            conteudo = (TCB_t *) GetAtIteratorFila2(fila);

            if(conteudo->tid == tid)
            {
                DeleteAtIteratorFila2(fila);

                removed = 0;
            }

            NextFila2(fila);

        }

    }

    return removed;
}

/******************************************************************************
Parâmetros:
	tid:	identificador da thread cujo término está sendo aguardado.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cjoin(int tid)
{
    if(__associarJoin(&g_filas[IDX_APTOS], g_emExecucao->tid, tid) == 0)
    {
        g_emExecucao->state = PROCST_BLOQ;

        AppendFila2(&g_filas[IDX_BLOQUEADOS], (void *)g_emExecucao);

        __dispatch();

        __removerDaFilPorTid(g_emExecucao->joined_tid, &g_filas[IDX_BLOQUEADOS]);

        __setupContextoPosDispatch();

        return 0;
    }
    else if(__associarJoin(&g_filas[IDX_BLOQUEADOS], g_emExecucao->tid, tid) == 0)
    {
        // TODO: Implementar caso de join numa thread bloqueada

        return -1;
    }

    return -1;
}

/******************************************************************************
Parâmetros:
	tid:	identificador da thread a ser suspensa.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csuspend(int tid)
{
    // Não implementado
    return -1;
}

/******************************************************************************
Parâmetros:
	tid:	identificador da thread que terá sua execução retomada.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cresume(int tid)
{
    // Não implementado
    return -1;
}

/******************************************************************************
Parâmetros:
	sem:	ponteiro para uma variável do tipo csem_t. Aponta para uma estrutura de dados que representa a variável semáforo.
	count: valor a ser usado na inicialização do semáforo. Representa a quantidade de recursos controlados pelo semáforo.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csem_init(csem_t *sem, int count)
{
    // Não implementado
    return -1;
}

/******************************************************************************
Parâmetros:
	sem:	ponteiro para uma variável do tipo semáforo.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cwait(csem_t *sem)
{
    // Não implementado
    return -1;
}

/******************************************************************************
Parâmetros:
	sem:	ponteiro para uma variável do tipo semáforo.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csignal(csem_t *sem)
{
    // Não implementado
    return -1;
}
