EXE = proxy
all: $(EXE)

CC = gcc
CFLAGS = -g -W -Wall
LIBS = -lpthread

OBJS = proxy.o aux_http.o  aux_socket.o priqueue.o

proxy: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIBS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) *.o *~ core.* 

