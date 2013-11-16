CC=gcc
CFLAGS=-Wall -O0 -ggdb
OBJS=pgps.o
EXE=bonus

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJS) $(EXE)

