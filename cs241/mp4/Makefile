#
# Machine Problem #4
# CS 241
#

CC = gcc
INC = -I.
FLAGS = -W -Wall -g

all: simulator queuetest

simulator: simulator.o libscheduler/libscheduler.o libpriqueue/libpriqueue.o
	$(CC) $^ -o $@

queuetest: queuetest.o libpriqueue/libpriqueue.o
	$(CC) $^ -o $@

queuetest.o: queuetest.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@

libscheduler/libscheduler.o: libscheduler/libscheduler.c libscheduler/libscheduler.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

libpriqueue/libpriqueue.o: libpriqueue/libpriqueue.c libpriqueue/libpriqueue.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@

simulator.o: simulator.c libscheduler/libscheduler.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@




.PHONY : clean
clean:
	-rm -f simulator queuetest *.o libscheduler/*.o libpriqueue/*.o
