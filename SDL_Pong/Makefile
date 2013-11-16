# Makefile for SDL Pong
#
# Author: Dennis J. McWherter, Jr.
# Date: 5 February 2011
#

CXX=g++
CXXFLAGS=-Wall -lSDL
OBJS=Pong.o main.o

all: pong

pong: $(OBJS)
	$(CXX) $(CXXFLAGS) -O2 -o pong Pong.o main.o

Pong.o:
	$(CXX) $(CXXFLAGS) -O2 -c -o Pong.o Pong.cpp

main.o: Pong.o
	$(CXX) $(CXXFLAGS) -O2 -c -o main.o main.cpp

clean:
	rm -rf *.o pong
