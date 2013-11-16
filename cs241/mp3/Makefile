# Machine Problem #3
# CS 241, Fall 2011
#

CC = gcc
INC = -I.
FLAGS = -W -Wall -g
LINKOPTS = -lpthread

all: mp3 gen

mp3: mp3.c
	$(CC) $(INC) $(FLAGS) $(LINKOPTS) mp3.c -o mp3

gen: gen.c
	$(CC) $(INC) $(FLAGS) gen.c -o gen

clean:
	rm -rf mp3 gen
