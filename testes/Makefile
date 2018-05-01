#
# Makefile para programas de teste
#

CC=gcc
LIB_DIR=../lib

all: teste_basico teste_semaforo

teste_basico: teste_basico.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_basico teste_basico.c -L$(LIB_DIR) -lcthread -Wall

teste_semaforo: teste_semaforo.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_semaforo teste_semaforo.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf teste_basico teste_semaforo *.o *~
