# MP3 Makefile
CXX=g++
CXXFLAGS=-Wall -O0 -ggdb
LIB=-pthread
INCLUDES=#-I/usr/lib/x86_64-redhat-linux5E/include
OBJS=tcp.o server.o client.o mp3.o
SERVER=mp3server
CLIENT=mp3client

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVER) $(LIB) tcp.o mp3.o server.o
	$(CXX) $(CXXFLAGS) -o $(CLIENT) $(LIB) tcp.o mp3.o client.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(SERVER) $(CLIENT) $(OBJS)

