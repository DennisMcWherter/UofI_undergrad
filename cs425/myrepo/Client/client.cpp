/**
 * Client.cpp
 *
 * Client-side application for MyRepo server
 */

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>

#include "ServerComponents/Protocol.h"

#include "Poco/Exception.h"
#include "Poco/Timespan.h"
#include "Poco/Timestamp.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"

using Poco::Exception;
using Poco::Timespan;
using Poco::Timestamp;
using Poco::Net::SocketAddress;
using Poco::Net::StreamSocket;

using namespace std;

bool beginsWith(const std::string& str, const std::string& compare)
{
  if(compare.size() > str.size())
    return false;

  for(unsigned i = 0 ; i < compare.size() ; ++i) {
    if(toupper(str[i]) != toupper(compare[i]))
      return false;
  }

  return true;
}

bool equals(const std::string& str, const std::string& compare)
{
  if(str.size() != compare.size())
    return false;

  for(unsigned i = 0 ; i < str.size() ; ++i) {
    if(toupper(str[i]) != toupper(compare[i]))
      return false;
  }

  return true;
}

void trim(std::string& str)
{
  std::string::iterator it = str.begin();
  std::string::iterator endWhite = str.end();

  // Trim forward whitespace
  while(*it == ' ' && it != str.end())
    str.erase(it);

  // Find
  for(; it != str.end() ; ++it) {
    if(*it == ' ' && endWhite == str.end())
      endWhite = it;
    else if(*it != ' ')
      endWhite = str.end();
  }

  // Trim trailing whitespace
  for(; endWhite != str.end() ;)
    str.erase(endWhite);
}

void replaceFirst(char repl, char with, std::string& str)
{
  unsigned idx = str.find_first_of(repl);
  if(idx == string::npos)
    return;
  str[idx] = with;
}

void importMovieList(string& input, Poco::Net::StreamSocket& sock)
{
  char ins = Protocol::INS_MOVIE;
  string line;
  unsigned space = input.find_first_of(' ');
  map<int, bool> printed;

  if(space == string::npos) {
    cout<< "Invalid request" << endl;
    return;
  }

  string file(input.substr(space + 1));

  ifstream fd(file.c_str());

  if(!fd.is_open()) {
    cout<< "Could not open file: "<< file << endl;
    return;
  }

  long start = fd.tellg();
  fd.seekg(0, ios::end);
  long fileSize = fd.tellg() - start;
  fd.seekg(0, ios::beg);

  long readBytes = 0;

  cout<< "Importing movie list from "<< file << " (this could take a while)" << endl;
  while(!fd.eof()) {
    getline(fd, line);

    unsigned length = line.size();

    if(length < 1)
      continue;

    readBytes += length;
    unsigned percent = ((double)readBytes / (double)fileSize) * 100;

    if(percent % 5 == 0 && !printed[percent]) {
      printed[percent] = true;
      cout<< "Percent complete: "<< percent<< "%" << endl;
    }

    sock.sendBytes(&ins, sizeof(ins));
    sock.sendBytes(&length, sizeof(length));
    sock.sendBytes(line.c_str(), length);
  }
  cout<< "Done" << endl;

  fd.close();
}

void handleInput(StreamSocket& sock)
{
  Timespan timeout(30, 0); // Wait up to 30 seconds for response (just in case there is extreme latency in processing)
  string input;
  char req;
  char resp = Protocol::REQ_NONE;
  char exit = Protocol::EXIT;

  cout<< "Connected to server."<< endl;

  sock.setReceiveTimeout(timeout);

  while(true) {
    getline(cin, input);

    if(input.empty())
      continue;

    trim(input);

    if(equals(input, "exit")) {
      sock.sendBytes(&exit, sizeof(exit));
      break;
    }

    unsigned pos = input.find_first_of(' ');

    if(pos == string::npos) {
      cout<< "Bad command" << endl;
      continue;
    }

    if(beginsWith(input, "insert")) {
      req = Protocol::INSERT;
    } else if(beginsWith(input, "lookup")) { 
      req = Protocol::LOOKUP;
    } else if(beginsWith(input, "remove")) {
      req = Protocol::REMOVE;
    } else if(beginsWith(input, "movieins")) {
      req = Protocol::INS_MOVIE;
    } else if(beginsWith(input, "movielup")) {
      req = Protocol::LUP_MOVIE;
    } else if(beginsWith(input, "importmovielist")) {
      importMovieList(input, sock);
      continue;
    } else {
      cout<< "Bad command" << endl;
      continue;
    }

    // Try to parse string
    string data(input.substr(pos+1));
    unsigned length = data.size();

    if(length == 0)
      continue;

    cout<< "Data length: "<< length << endl;

    // Initiate request
    Timestamp theTime;
    sock.sendBytes(&req, sizeof(req));
    sock.sendBytes(&length, sizeof(length));
    sock.sendBytes(data.c_str(), length);

    if(req != Protocol::INS_MOVIE)
      sock.receiveBytes(&resp, sizeof(resp)); // Receive response

    if(resp == Protocol::REQ_FAIL) {
      cout<< "Request failed" << endl;
    } else if(resp == Protocol::REQ_SUC || resp == Protocol::LOOKUP_SUC) {
      cout<< "Request successful" << endl;
      if(req == Protocol::INSERT || req == Protocol::REMOVE) {
        cout<< "!" << endl;
      } else if(req == Protocol::LOOKUP || req == Protocol::LUP_MOVIE) {
        unsigned length = 0;
        unsigned totalReceived = 0;

        sock.receiveBytes(&length, sizeof(length));

        unsigned rem = length;

        char* buf = new char[length+1];
        char* msg = buf;
        buf[length] = '\0';

        while((totalReceived += sock.receiveBytes(msg, rem)) != length) {
          msg = buf + totalReceived;
          rem = length - totalReceived;
        }

        Timestamp::TimeDiff diff(theTime.elapsed());
        float timeDiff = ((float)diff / (float)1000);

        cout<< ": " << buf << endl;

        string throwaway;
        stringstream strstream;
        strstream << buf;
        if(req == Protocol::LUP_MOVIE) {
          int count = 0;
          while(getline(strstream, throwaway)) count++;
          cout<< "Movie(s) found: "<< count << " in "<< timeDiff << "ms" << endl;
        }

        delete [] buf;
      }/* else if(req == Protocol::LUP_MOVIE && false) {
        cout<< "Looking up movie" << endl;
        int count = 0;
        while(resp != Protocol::MLOOKUP_DONE) {
          if(resp == Protocol::LOOKUP_NO) {
            cout<< "No movies found with that word" << endl;
            break;
          } else if(resp == Protocol::LOOKUP_SUC) {
            count++;
            sock.receiveBytes(&length, sizeof(length));
            char* buf = new char[length + 1];
            buf[length] = '\0';
            sock.receiveBytes(buf, length);
            cout<< buf << endl;
            delete [] buf;
          } else if(resp == Protocol::MLOOKUP_DONE) {
            break; // just in case
          }
          sock.receiveBytes(&resp, sizeof(resp));
        }
        cout<< "Movie(s) found: "<< count << endl;
      }*/
    } else if(resp == Protocol::LOOKUP_NO) {
      Timestamp::TimeDiff diff(theTime.elapsed());
      float timeDiff = ((float)diff / (float)1000);
      cout<< "No data exists at that id" << " in "<< timeDiff << "ms" << endl;
    }

    resp = Protocol::REQ_NONE;
  }
}

int main(int argc, char** argv)
{
  if(argc < 3) {
    cerr << "Usage: "<< argv[0] << " <server_addr> <server_port>" << endl;
    return 1;
  }

  Timespan timeout(3, 0); // Try to connect for 3 seconds
  SocketAddress sock(argv[1], argv[2]);
  StreamSocket conn;
  char resp;

  try {
    conn.connect(sock, timeout);
    cout<< "Trying to connect to server..." << endl;
    
    conn.receiveBytes(&resp, sizeof(resp));

    if(resp == Protocol::SRV_CONN)
      handleInput(conn);
    else if(resp == Protocol::SRV_FULL)
      cout<< "Server full. Disconnecting." << endl;
    else
      cout<< "Unknown response while trying to connect"<< endl;

  } catch(Exception& e) {
    cout<< "Exception: "<< e.displayText() << endl;
  }

  conn.close();

  return 0;
}
