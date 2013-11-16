CC=gcc
CFLAGS=-Wall -O0 -ggdb
LIBS=-lpthread
OBJS=mp1_base.o mp1util.o mp1client.o ConcurrentQueue.o
EXE=mp1client
TESTSRC=testqueue.c ConcurrentQueue.c

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TESTSRC)
	$(CC) $(CFLAGS) -o test $(TESTSRC) $(LIBS)

clean:
	rm -rf $(OBJS) $(EXE) test

