#
# Makefile para programas de teste
#

CC=gcc
LIB_DIR=../lib

all: teste

teste: teste.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste teste.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf teste *.o *~
