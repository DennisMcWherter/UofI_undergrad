# Simple Makefile for MP1
# CS425
#

CC=gcc
CFLAGS=-Wall -g -O0
OUTPUT=bin/mykv
TESTING=tests/test
OBJS=util.o Config/Config.o Networking/Server.o Networking/Client.o Networking/Sockets.o Networking/HandleClient.o \
		 Database/MemDB.o
LIBS=-lpthread

all: main.o $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS) main.o $(LIBS)

testing: $(OBJS)
	$(CC) $(CFLAGS) -o $(TESTING) $(OBJS) tests/testing.c $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(OBJS) $(OUTPUT) $(TESTING)

