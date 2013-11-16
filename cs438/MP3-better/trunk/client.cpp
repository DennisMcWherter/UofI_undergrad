/**
 * client.cpp
 *
 * Client implementation
 *
 * @author Dennis J. McWherter, Jr.
 */

#include <iostream>
#include <fstream>

#include "tcp.h"

#define MAX_BUFFER (75)

using namespace std;

static void receiveFile(TCP&, int, ofstream&);

int main(int argc, char* argv[])
{
  if(argc < 4) {
    cerr << "Usage: "<< argv[0] << " <server name> <server port> <output file>" << endl;
    return -1;
  }

  TCP client(argv[1], argv[2]);
  int sockfd = -1;

  if((sockfd = client.connect()) < 0) {
    cerr<< "Could not establish connection with server " << argv[1] << ":" << argv[2] << endl;
    return -1;
  }

  ofstream out(argv[3], ios::binary | ios::trunc);

  if(!out.is_open()) {
    cerr<< "Could not open file for writing: "<< argv[3] << endl;
    return -1;
  }

  receiveFile(client, sockfd, out);

  out.close();

  return 0;
}

void receiveFile(TCP& client, int sockfd, ofstream& out)
{
  char buffer[MAX_BUFFER];
  int read = 0;

//  char buf[10];
  //int i = 0;
  while((read = client.recv(sockfd, buffer, MAX_BUFFER)) > 0) {
    out.write(buffer, read);
    cout<< buffer;
    //i++;
    //int v = sprintf(buf, "%d", i);
    //out.write(buf, v);
  }
}

