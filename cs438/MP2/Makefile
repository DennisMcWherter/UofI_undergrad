CC=gcc
CFLAGS=-Wall -O0 -ggdb
LIBS=-lpthread
OBJS=router.o networking.o protocol.o ConcurrentQueue.o graph.o
EXE=router
#TESTSRC=testqueue.c ConcurrentQueue.c

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#test: $(TESTSRC)
#	$(CC) $(CFLAGS) -o test $(TESTSRC) $(LIBS)

clean:
	rm -rf $(OBJS) $(EXE) test

