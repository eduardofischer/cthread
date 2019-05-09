#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

all: scheduler semaphore

scheduler: scheduler_test.c $(LIB_DIR)/libcthread.a
	$(CC) -o scheduler_test scheduler_test.c -L$(LIB_DIR) -lcthread -Wall

semaphore: semaphore_test.c $(LIB_DIR)/libcthread.a
	$(CC) -o semaphore_test semaphore_test.c -L$(LIB_DIR) -lcthread -Wall
	
clean:
	rm -rf scheduler_test semaphore_test *.o *~




