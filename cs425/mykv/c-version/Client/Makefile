# Simple Makefile for MP2
# CS425
#

CC=gcc
CFLAGS=-Wall -g -O0
OUTPUT=bin/client
OBJS=../Networking/Sockets.o
LIBS=

all: client.o $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS) client.o $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(OBJS) $(OUTPUT)

