# CS 241
# University of Illinois

CC = gcc
INC = -I.
FLAGS = -g -W -Wall
LIBS = -lpthread

OBJECTS = libdictionary.o libmapreduce.o


all: test1 test2 test3 test4 test5

test1: $(OBJECTS) test1.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test2: $(OBJECTS) test2.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test3: $(OBJECTS) test3.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test4: $(OBJECTS) test4.c 11.txt
	$(CC) $(FLAGS) test4.c $(OBJECTS) -o $@ $(LIBS)

test5: $(OBJECTS) test5.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)


libdictionary.o: libdictionary.c libdictionary.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

libmapreduce.o: libmapreduce.c libmapreduce.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)



11.txt:
	wget -nv http://www.gutenberg.org/files/11/11.zip
	unzip 11.zip
	$(RM) 11.zip


clean:
	$(RM) *.o *.d test1 test2 test3 test4 test5 *~
