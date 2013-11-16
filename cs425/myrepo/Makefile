# MyRepo Makefile
#
CXX=g++
CXXFLAGS=-Wall -g -O0
INCLUDE=-Ilibs/poco-1.4.3p1/Foundation/include -Ilibs/poco-1.4.3p1/Net/include -Ilibs/poco-1.4.3p1/XML/include
LIBDIR=-Llibs/poco-1.4.3p1/lib
LIBS=-lPocoFoundationd -lPocoNetd -lPocoXMLd -Wl,-rpath=`pwd`/bin
EXE=bin/myrepo
OBJS=main.o Server.o SignalHandler.o Config/Config.o Config/ConfigReader.o Config/ConfigHandler.o ServerComponents/FailureDetector.o ServerComponents/KeyValueStore.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(INCLUDE) $(LIBDIR) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)

install:
	cp -rf libs/poco-1.4.3p1/lib/libPocoFoundation* libs/poco-1.4.3p1/lib/libPocoNet* libs/poco-1.4.3p1/lib/libPocoXML* bin/

clean:
	rm -rf $(EXE) $(OBJS) bin/*.so* bin/*.a

