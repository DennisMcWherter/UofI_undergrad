/**
 * server.cpp
 *
 * Server implementation
 *
 * @author Dennis J. McWherter, Jr.
 */

#include <cstring>
#include <iostream>
#include <fstream>

#include "tcp.h"

#define MAX_READ (75) // In bytes

using namespace std;

static void sendData(TCP&, int, ifstream&);

int main(int argc, char* argv[])
{
  if(argc < 3) {
    cerr<< "Usage: "<< argv[0] << " <server port> <file name>" << endl;
    return -1;
  }

  TCP server(NULL, argv[1], true);
  if(server.listen() < 0) {
    cerr<< "Error: Could not initialize server socket." << endl;
    return -1;
  }

  ifstream in(argv[2], ios::binary);
  if(!in.is_open()) {
    cerr<< "Error: Could not open file for reading: "<< argv[2] << endl;
    return -1;
  }

  while(1) {
    int sockfd = server.accept();

    if(sockfd > 0) {
      sendData(server, sockfd, in); 
      // Do not close sockfd since we will continue to use it
      // for all connections
      // However, tell the other connection to close
      server.closeClient(sockfd);
    }
  }

  in.close();

  return 0;
}

void sendData(TCP& server, int sockfd, ifstream& in)
{
  unsigned bufSize = 2 * MAX_READ;
  char* buffer = new char[bufSize];
  unsigned totalRead = 0;
  streamsize read = 0;
  cout<< "Sending data... " << endl;
  while(!in.eof()) {
    in.read(buffer + totalRead, MAX_READ);
    streamsize size = in.gcount();
    totalRead += size;
    if((totalRead + MAX_READ) >= bufSize) {
      bufSize *= 2;
      char* tmp = new char[bufSize];
      memcpy(tmp, buffer, totalRead);
      delete [] buffer;
      buffer = tmp;
    }
  }
  cout<< "Double check: "<< buffer << endl;
  server.send(sockfd, buffer, totalRead);
  in.clear();
  in.seekg(0, ios::beg);
  cout<< "Done" << endl;
  delete [] buffer;
}

