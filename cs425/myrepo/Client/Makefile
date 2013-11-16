# MyRepo Makefile
#
CXX=g++
CXXFLAGS=-Wall -g -O0
INCLUDE=-I../libs/poco-1.4.3p1/Foundation/include -I../libs/poco-1.4.3p1/Net/include -I../libs/poco-1.4.3p1/XML/include -I../
LIBDIR=-L../libs/poco-1.4.3p1/lib
LIBS=-lPocoFoundationd -lPocoNetd -Wl,-rpath=`pwd`/../bin
EXE=../bin/client
OBJS=client.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(INCLUDE) $(LIBDIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)

clean:
	rm -rf $(EXE) $(OBJS)

