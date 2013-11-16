/**
 * KeyValueStore.cpp
 *
 * Key value store implementation
 */

#include "KeyValueStore.h"
#include "Protocol.h"
#include "../ConcurrentQueue.hpp"
#include "../MemDB.hpp"
#include "../Config/Config.h"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Poco
#include "Poco/Condition.h"
#include "Poco/Exception.h"
#include "Poco/Mutex.h"
#include "Poco/Semaphore.h"
#include "Poco/TaskManager.h"
#include "Poco/Thread.h"
#include "Poco/Timespan.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"

using Poco::Condition;
using Poco::Exception;
using Poco::Mutex;
using Poco::Semaphore;
using Poco::Task;
using Poco::TaskManager;
using Poco::Thread;
using Poco::TimeoutException;
using Poco::Timespan;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Net::StreamSocket;

using namespace std;

#define MAX_CLIENT_THREADS 10 // Up to 10 clients 
#define printKv(x) (cout<< "KVServer: "<< x << endl)
#define notUsed(x) // Silence the compiler warnings

// Global vars are bad - but this is a quicker solution when pressed for time
MemDB<vector<string> > movieDB;
MemDB<string> memDB;
ConcurrentQueue<pair<char, string> > forwardQueue;
ConcurrentQueue<pair<char, string> > respBuffer;
Semaphore freeThreads(MAX_CLIENT_THREADS);
Mutex procForward;
Condition forwardCond;

KeyValueStore::KeyValueStore(Config& config)
  : config(config)
{
}

KeyValueStore::~KeyValueStore()
{
}

void KeyValueStore::run()
{
  Thread threads[3];
  InternalThread inInThread(config, true);
  InternalThread inOutThread(config, false);
  PublicThread pubThread(config);

  threads[0].start(pubThread);
  threads[1].start(inInThread);
  threads[2].start(inOutThread);

  threads[0].join();
  threads[1].join();
  threads[2].join();
}

/* Utility functions */
int KeyValueStore::stringToInt(const string& str)
{
  int ret = 0;
  stringstream ss;
  ss << str;
  ss >> ret;
  return ret;
}

string KeyValueStore::intToString(int val)
{
  string ret;
  stringstream ss;
  ss << val;
  ss >> ret;
  return ret;
}

string KeyValueStore::extractQuotes(const string& str)
{
  string ret(str);
  string::iterator it = ret.begin();
  bool open = false;

  int numChars = 0;
  for(; it != ret.end() ; ++it) {
    if(*it == '"') {
      if(open) {
        numChars++;
        break;
      } else {
        open = true;
        it--;
        ret.erase(it);
      }
    } else {
      numChars++;
    }
  }

  return ret.substr(0, numChars);
}

vector<string> KeyValueStore::tokenize(const string& str, char delim)
{
  vector<string> ret;
  stringstream ss(str);
  string s;

  while(getline(ss, s, delim))
    ret.push_back(s);

  return ret;
}

void KeyValueStore::insertMovieDB(StreamSocket& client, const string& buf, Config::ThreadControl& control, bool forward)
{
  vector<string> tmp;
  tmp.push_back(buf);
  string title(KeyValueStore::extractQuotes(buf));

  vector<string> tokens(KeyValueStore::tokenize(title));
  movieDB.lock();
  for(vector<string>::iterator it = tokens.begin() ; it != tokens.end(); ++it) {
    if(!forward || isResponsible(*it, control)) {
      map<string, vector<string> >::iterator itt = movieDB.rawLookup(*it);
      if(itt == movieDB.rawEnd()) {
    //    printKv("Insert movie");
        movieDB.rawInsert(*it, tmp);
      } else {
      //  printKv("Updated movie");
        itt->second.push_back(buf);
      }
    } else {
      printKv("Forwarding movie");
      pair<char, string> resp(KeyValueStore::forwardRequest(Protocol::INS_MOVIE, buf, control));
      client.sendBytes(&resp.first, sizeof(resp.first));
    }
  }
  movieDB.unlock();
}

/** Handling requests */
bool KeyValueStore::handleRequest(StreamSocket& client, Config::ThreadControl& control, char*& buf)
{
  char req;
  unsigned contentLength = 0;

  client.receiveBytes(&req, sizeof(req));

  if(req == Protocol::EXIT) {
    return true;
  }

  // Always expect that content-length will be the next sent.
  client.receiveBytes(&contentLength, sizeof(contentLength));
  if(contentLength < 0)
    throw Exception("Invalid content-length detected. Dropping client");

  buf = new char[contentLength+1];
  buf[contentLength] = '\0';
  client.receiveBytes(buf, contentLength);

  KeyValueStore::handleQuery(client, control, req, buf);

  delete [] buf;
  buf = NULL;

  return false;
}

void KeyValueStore::handleQuery(StreamSocket& client, Config::ThreadControl& control, char req, const string& buf)
{
  if(buf.empty())
    return; // Should not happen?

  // If we handle the data, do some magic
  if(req == Protocol::INSERT) {
    KeyValueStore::handleInsert(client, buf, control);
  } else if(req == Protocol::REMOVE) {
    KeyValueStore::handleRemove(client, buf, control);
  } else if(req == Protocol::LOOKUP) {
    KeyValueStore::handleLookup(client, buf, control);
  } else if(req == Protocol::INS_MOVIE) {
    KeyValueStore::handleInsertMovie(client, buf, control);
  } else if(req == Protocol::LUP_MOVIE) {
    KeyValueStore::handleLookupMovie(client, buf, control);
  } else if(req == Protocol::REPL_INS || req == Protocol::REPL_REM || req == Protocol::REPL_MOVIE) {
    unsigned space = buf.find_first_of(' ');
    if(space == string::npos) {
      printKv("Invalid replication request received");
      return;
    }
    int count = KeyValueStore::stringToInt(buf.substr(0, space));
    if(count < 0)
      return; // Paranoia - this should never be sent to us

    string rem(buf.substr(space+1));

    if(req == Protocol::REPL_INS) {
      KeyValueStore::handleInsRepl(rem, count, control);
    } else if(req == Protocol::REPL_REM) {
      KeyValueStore::handleRemRepl(rem, count, control);
    } else if(req == Protocol::REPL_MOVIE) {
      KeyValueStore::handleMovieInsRepl(rem, count, control);
    }
  }
}

void KeyValueStore::handleInsertMovie(StreamSocket& client, const string& buf, Config::ThreadControl& control)
{
  KeyValueStore::insertMovieDB(client, buf, control);
  // Upon insert, do not reply to sender
}

void KeyValueStore::handleMovieInsRepl(const string& rem, int count, Config::ThreadControl& control)
{
  static StreamSocket placeHolder;
  static long numRec = 0;
  numRec++;

  if(numRec % 100000 == 1)
    printKv("REPL_MOVIE request received (" << dec << numRec << ")");

  count--;
  
  KeyValueStore::insertMovieDB(placeHolder, rem, control, false); 
}

/** NOT movie database */

void KeyValueStore::handleLookupMovie(StreamSocket& client, const string& buf, Config::ThreadControl& control)
{
  static const char suc  = Protocol::LOOKUP_SUC;
  static const char no   = Protocol::LOOKUP_NO;
  static const char done = Protocol::MLOOKUP_DONE;
  string ret;
  unsigned sent = 0;
  const char* msg = NULL;
  map<string, bool> isSent;

  printKv("Received movie lookup request");

  if(isResponsible(buf, control)) {
    movieDB.lock();
    map<string, vector<string> >::iterator it = movieDB.rawLookup(buf);
    printKv("Hi, I'm a server and I am serving this request: " << buf);
    if(it == movieDB.rawEnd()) {
      client.sendBytes(&no, sizeof(no));
    } else {
      vector<string>::iterator vit = it->second.begin();
      for(; vit != it->second.end(); ++vit) {
        if(!isSent[*vit]) {
          ret.append(*vit);
          ret.append("\n");
          isSent[*vit] = true;
        }
      }
      client.sendBytes(&suc, sizeof(suc));
      unsigned length = ret.size();
      unsigned rem = length;
      unsigned totalSent = 0;
      client.sendBytes(&length, sizeof(length));
      msg = ret.c_str();
      printKv("Sending...");
      while((totalSent += client.sendBytes(msg, rem)) != length) {
        msg = ret.c_str() + totalSent;
        rem = length - totalSent;
        printKv("Remaining: "<< rem);
      }
      //client.sendBytes(&done, sizeof(done));
      printKv("Done sending");
    }
    movieDB.unlock();
  } else {
    //vector<pair<char, string> > resps(KeyValueStore::forwardRequest(Protocol::LUP_MOVIE, buf, control));
    pair<char, string> resps(KeyValueStore::forwardRequest(Protocol::LUP_MOVIE, buf, control));

#if 0
    vector<pair<char, string> >::iterator it = resps.begin();
    for(; it != resps.end() ; ++it) {
      if(it->first == Protocol::LOOKUP_SUC) {
        client.sendBytes(&it->first, sizeof(it->first));
        unsigned length = it->second.size();
        client.sendBytes(&length, sizeof(length));
        //it++;
        client.sendBytes(it->second.c_str(), length);
        printKv("Protocol::LOOKUP_SUC");
      } else {
        client.sendBytes(&it->first, sizeof(it->first));
        if(it->first == Protocol::LOOKUP_NO)
          printKv("Protocol::LOOKUP_NO");
        else if(it->first == Protocol::MLOOKUP_DONE)
          printKv("Protocol::MLOOKUP_DONE");
      }
    }
#else
    string str(resps.second);
    //printKv(str);
    unsigned length = str.size();
    unsigned rem = length;
    unsigned totalSent = 0;
    const char* msg = str.c_str();

    client.sendBytes(&resps.first, sizeof(resps.first));

    if(resps.first == Protocol::LOOKUP_SUC) {
      client.sendBytes(&length, sizeof(length));
      while((totalSent += client.sendBytes(msg, rem)) != length) {
        msg = str.c_str() + totalSent;
        rem = length - totalSent;
        printKv("Remaining: "<< rem);
      }
    }
#endif
  }
  
  printKv("MovieDB size: "<< movieDB.size());
}

void KeyValueStore::handleInsert(StreamSocket& client, const string& buf, Config::ThreadControl& control)
{
  static const char sucResp  = Protocol::REQ_SUC;
  static const char failResp = Protocol::REQ_FAIL;
  const char* ptr = buf.c_str();

  while(*ptr != '\0' && *ptr != '\r' && *ptr != '\n' && *ptr != ' ') ptr++;

  if(*ptr != ' ') {
    printKv("Invalid request");
    client.sendBytes(&failResp, sizeof(failResp));
    return; // Invalid
  }

  string key(buf.substr(0, (ptr - buf.c_str())));
  string data(buf.substr((ptr - buf.c_str()) + 1));

  if(isResponsible(key, control)) {
    printKv("INSERT request received");
    printKv("Received key: "<< key << endl << "Value: "<< data);

    memDB.insert(key, data);

    // Respond to client
    if(data.compare(memDB.lookup(key)) != 0)
      client.sendBytes(&failResp, sizeof(failResp));
    else
      client.sendBytes(&sucResp, sizeof(sucResp));

    KeyValueStore::doReplicate(Protocol::REPL_INS, buf, control);
  } else {
    pair<char, string> resp(KeyValueStore::forwardRequest(Protocol::INSERT, buf, control));
    client.sendBytes(&resp.first, sizeof(resp.first));
  }
}

void KeyValueStore::handleRemove(StreamSocket& client, const string& buf, Config::ThreadControl& control)
{
  static const char sucResp  = Protocol::REQ_SUC;
  static const char failResp = Protocol::REQ_FAIL;

  printKv("REMOVE request received");

  if(isResponsible(buf, control)) {
    memDB.remove(buf);

    if(memDB.lookup(buf).empty())
      client.sendBytes(&sucResp, sizeof(sucResp));
    else
      client.sendBytes(&failResp, sizeof(failResp));

    KeyValueStore::doReplicate(Protocol::REPL_REM, buf, control, control.getMachineCount());
  } else {
    pair<char, string> resp(KeyValueStore::forwardRequest(Protocol::REMOVE, buf, control));
    client.sendBytes(&resp.first, sizeof(resp.first));
  }
}

void KeyValueStore::handleLookup(StreamSocket& client, const string& buf, Config::ThreadControl& control)
{
  static const char noLookup = Protocol::LOOKUP_NO;
  static const char scLookup = Protocol::LOOKUP_SUC;

  if(isResponsible(buf, control)) {
    string str(memDB.lookup(buf));
    printKv("LOOKUP request received");

    if(!str.empty()) {
      printKv("Handling LOOKUP");
      unsigned contentLength = str.size();
      client.sendBytes(&scLookup, sizeof(scLookup)); // Respond
      client.sendBytes(&contentLength, sizeof(contentLength)); // How much info
      client.sendBytes(str.c_str(), contentLength);
    } else {
      client.sendBytes(&noLookup, sizeof(noLookup));
    }
  } else {
    printKv("Forwarding LOOKUP");
    pair<char, string> resp(KeyValueStore::forwardRequest(Protocol::LOOKUP, buf, control));
    client.sendBytes(&resp.first, sizeof(resp.first));
    if(resp.first != Protocol::LOOKUP_NO && resp.first != Protocol::REQ_FAIL) {
      unsigned size = resp.second.size();
      client.sendBytes(&size, sizeof(size));
      client.sendBytes(resp.second.c_str(), size);
    }
  }
}

void KeyValueStore::handleInsRepl(const string& rem, int count, Config::ThreadControl& control)
{
  printKv("REPL_INS request received");
  unsigned space = rem.find_first_of(' ');
  if(space == string::npos) {
    printKv("Invalid replication (insert) request received: "<< rem);
    return;
  }

  string key(rem.substr(0, space));
  string data(rem.substr(space+1));

  if(isResponsible(key, control)) {
    printKv("Responsible for key - not replicating");
    return;
  }

  count--;
  
  if(!isResponsible(key, control))
    memDB.insert(key, data); 

  if(data.compare(memDB.lookup(key)) != 0)
    printKv("Error in replicating (insertion)");

  if(count > 0) { // Forward the request
    KeyValueStore::doReplicate(Protocol::REPL_INS, rem, control, count);
  }
}

void KeyValueStore::handleRemRepl(const string& rem, int count, Config::ThreadControl& control)
{
  printKv("REPL_REM request received");

  count--;

  // If we are responsible, cannot be sure that this is a valid message. Let it die.
  if(!isResponsible(rem, control)) {
    memDB.remove(rem);

    if(!memDB.lookup(rem).empty())
      printKv("Error in replication (removal)");

    if(count > 0) {
      KeyValueStore::doReplicate(Protocol::REPL_REM, rem, control, count);
    }
  }
}

string KeyValueStore::createReplMsg(const string& req, int remaining)
{
  string msg(intToString(remaining));
  msg.append(" ");
  msg.append(req);
  return msg;
}

bool KeyValueStore::isResponsible(const string& key, Config::ThreadControl& control)
{
  unsigned currId = control.getPublicId();
  unsigned mCount = control.getMachineCount();
  int numKeys = 100000000; // Some very large number

  if(mCount == 0)
    return false; // Connecting

  if(mCount == 1)
    return true; // No other machines connected

  unsigned keysPerRange = numKeys / mCount;

  unsigned hash = MemDB<unsigned>::getHash(key, numKeys);

  unsigned lbound = keysPerRange * currId;
  unsigned ubound = keysPerRange * (currId + 1);

  return (hash >= lbound && hash <= ubound);
}

void KeyValueStore::doReplicate(Protocol::KEYVALUE_STORE type, const string& req, Config::ThreadControl& control, int remaining)
{
  Mutex& mutex = control.getOutMutex();

  // First craft message
  pair<char, string> entry(type, KeyValueStore::createReplMsg(req, remaining));

  forwardQueue.enqueue(entry);

  mutex.lock();
  control.getOutCondition().signal();
  mutex.unlock();
}

void KeyValueStore::replicateDB(StreamSocket& sock, unsigned mCount)
{
  map<int, bool> printed;

  printKv("Replicating database on new machine");
  // Could potentially take some time, we are going to lock the DB
  memDB.lock();
  map<string, string>::const_iterator it = memDB.begin();
  for(; it != memDB.end() ; ++it) {
    string req(it->first);
    req.append(" ");
    req.append(it->second);
    pair<char, string> request(Protocol::REPL_INS, KeyValueStore::createReplMsg(req));
    KeyValueStore::sendRequest(sock, request);
  }
  memDB.unlock();

  // This will most likely take a lot of time - replicate movie DB
  movieDB.lock();
  long count = 0;
  long size  = 0;
  map<string, vector<string> >::const_iterator itt = movieDB.begin();
  printKv("Counting for MovieDB progress");
  for(; itt != movieDB.end() ; ++itt) { // Count how many entries for progress
    size += itt->second.size();
  }
  printKv("Replicating MovieDB... (this will take a while)");
  for(itt = movieDB.begin() ; itt != movieDB.end() ; ++itt) {
    vector<string>::const_iterator vit = itt->second.begin();
    vector<string>::const_iterator end = itt->second.end();
    for(; vit != end; ++vit) {
      unsigned forward = (mCount > 2) ? 2 : 1;
      pair<char, string> req(Protocol::REPL_MOVIE, KeyValueStore::createReplMsg(*vit, forward));
      KeyValueStore::sendRequest(sock, req);
      count++;
    }
    unsigned percent = ((double)count / (double)size) * 100;
    if(percent % 5 == 0 && printed[percent] == false) {
      printed[percent] = true;
      printKv(dec << percent <<"% complete replicating movie database");
    }
  }
  movieDB.unlock();
}

pair<char, string> KeyValueStore::forwardRequest(Protocol::KEYVALUE_STORE type, const string& req, Config::ThreadControl& control)
{
  Mutex& outMutex = control.getOutMutex();
  Condition& cond = control.getOutCondition();

  procForward.lock();
  pair<char, string> freq(type, req);
  forwardQueue.enqueue(freq);
  outMutex.lock();
  cond.signal();
  outMutex.unlock();
  forwardCond.wait(procForward);
  pair<char, string> resp(respBuffer.dequeue());
  procForward.unlock();

  return resp;
}

// Actually only used for movie requests
vector<pair<char, string> > KeyValueStore::forwardLongRequest(Protocol::KEYVALUE_STORE type, const string& req, Config::ThreadControl& control)
{
  vector<pair<char, string> > ret;
  Mutex& outMutex = control.getOutMutex();
  Condition& cond = control.getOutCondition();

  procForward.lock();
  pair<char, string> freq(type, req);
  forwardQueue.enqueue(freq);
  
  outMutex.lock();
  cond.signal();
  outMutex.unlock();
  forwardCond.wait(procForward);

  while(true) {
    pair<char, string> resp(respBuffer.dequeue());
    ret.push_back(resp);
    if(resp.first == Protocol::MLOOKUP_DONE || resp.first == Protocol::LOOKUP_NO) {
      procForward.unlock();
      break;
    }
  }

  printKv("Done serving movie list");

  return ret;
}

void KeyValueStore::sendRequest(StreamSocket& sock, const pair<char, string>& req)
{
  unsigned contentLength = req.second.size();
  unsigned rem = contentLength;
  unsigned totalSent = 0;

  sock.sendBytes(&req.first, sizeof(req.first));
  sock.sendBytes(&contentLength, sizeof(contentLength));
  
  const char* msg = req.second.c_str();

  while((totalSent += sock.sendBytes(msg, rem)) != contentLength) {
    msg = req.second.c_str() + totalSent;
    rem = contentLength - totalSent;
  }
}

pair<char, string> KeyValueStore::rcvResponse(StreamSocket& sock, char*& buf)
{
  unsigned contentLength = 0;
  char resp;
  pair<char, string> ret;
  char* msg = NULL;

  sock.receiveBytes(&resp, sizeof(resp));

  if(resp == Protocol::LOOKUP_SUC) {
    sock.receiveBytes(&contentLength, sizeof(contentLength));

    unsigned rem = contentLength;
    unsigned totalRead = 0;

    buf = new char[contentLength+1];
    msg = buf;
    buf[contentLength] = '\0';

    while((totalRead += sock.receiveBytes(msg, rem)) != contentLength) {
      msg = buf + totalRead;
      rem = contentLength - totalRead;
    }
    
    ret = pair<char, string>(resp, buf);
    delete [] buf;
    buf = NULL;
  } else {
    ret = pair<char, string>(resp, "");
  }

  return ret;
}

/** Public thread methods (clients connect here) */
void KeyValueStore::PublicThread::run()
{
  Thread threads[MAX_CLIENT_THREADS];
  HandleClient threadInst[MAX_CLIENT_THREADS];
  const Config::ServerInformation& info = config.getServerInformation();
  Config::ThreadControl& control = config.getThreadControl();
  int id = 0;
  char full = Protocol::SRV_FULL;
  char conn = Protocol::SRV_CONN;

  ServerSocket server;
  SocketAddress sock(info.address, info.pubPort);
  server.bind(sock, true);
  server.listen(5);

  printKv("Listening for clients on "<< info.address <<":"<< info.pubPort);

  while(control.isLive()) {
    // Simply do thread per client
    StreamSocket client = server.acceptConnection();
    printKv("Received client connection request - waiting for thread to free up");
    
    // Wait five seconds
    try {
      freeThreads.wait(5000); // This beats busy waiting
    } catch(TimeoutException& notUsed(e)) {
      printKv("Server full - closing connection to client");
      client.sendBytes(&full, sizeof(full));
      client.close();
      continue;
    }

    // Send success
    client.sendBytes(&conn, sizeof(conn));

    // tryJoin() doesn't work properly in linux, using isRunning() instead
    // actively search for the next available thread
    while(threads[id].isRunning()){ // Try to get an available thread
      id = (id + 1) % MAX_CLIENT_THREADS;
      Thread::sleep(250); // 250ms between each check
    }

    printKv("Serving client");
    threadInst[id] = HandleClient(client, control);
    threads[id].start(threadInst[id]);
  }

  server.close();
  freeThreads.set(); // Free a thread with semaphore
}

  /** Public thread handle client routine */
void KeyValueStore::PublicThread::HandleClient::run()
{
  if(control == NULL) {
    printKv("Cannot server client - no access to thread control");
    return; // This should never happen
  }

  char* buf = NULL;

  printKv("Client connected");

  while(true) {
    try {
      try {
        if(KeyValueStore::handleRequest(client, *control, buf))
          break;
      } catch(TimeoutException& notUsed(e)) {
        // Simply guarantee liveness for invalid requests
        if(buf != NULL)
          delete [] buf;
      }
    } catch(Exception& e) {
      printKv(e.displayText());
      break;
    }
  }

  if(buf != NULL)
    delete [] buf;

  client.close();
  printKv("Client disconnected");
}

/** Internal thead methods (servers connect here) */
void KeyValueStore::InternalThread::run()
{
  if(incoming) {
    try {
      incomingRoutine();
    } catch(Exception& e) {
      printKv("Exception caught (internal incoming): "<< e.displayText());
    }
  } else {
    try {
      outgoingRoutine();
    } catch(Exception& e) {
      printKv("Exception caught (internal outgoing): "<< e.displayText());
    }
  }
}

void KeyValueStore::InternalThread::incomingRoutine()
{
  const Config::ServerInformation& info = config.getServerInformation();
  Config::ThreadControl& control = config.getThreadControl();
  Mutex& inMutex = control.getInMutex();
  Condition& cond = control.getInCondition();
  Thread incoming;
  Incoming inTask;
  int currId = -1;
  static bool first = true;

  // Init connection info
  SocketAddress sock(info.address, info.internalPort);
  ServerSocket server;
  try {
    server.bind(sock, true);
    server.listen(5);
  } catch(Exception& e) {
    printKv("Could not initialize internal thread, please restart server ("<< e.displayText()<< ")");
  }

  StreamSocket client;

  while(control.isLive()) {
    inMutex.lock();
    cond.wait(inMutex);
    unsigned nextId = control.getConnectedId();
    inMutex.unlock();

    // NOTE: From a security perspective this is not safe.
    //  if someone tries to connect at the same time a rejoin
    //  was initiated, they could easily perform a MITM attack.
    //  However, since this is an academic exercise, I am not too
    //  concerned with security (as can be seen by many other components
    //  in this system as well).
    if(currId != (int)nextId) {
      currId = nextId;
      // TODO: Update processing thread somehow
      printKv("Told a new server should be connecting...");
      try {
        client = server.acceptConnection();
        printKv("Incoming server connected: "<< currId);
        inTask.cancel();
        if(!first)
          incoming.join();
        first = false;
        inTask = Incoming(client, &config.getThreadControl());
        incoming.start(inTask);
        printKv("Handling new server");
      } catch(TimeoutException& e) {
        printKv("Server did not connect in time - we don't want the system to be hung up, though ("<< e.displayText() <<")");
      }
    }
  }

  server.close();
}

void KeyValueStore::InternalThread::Incoming::runTask()
{
  if(control == NULL) {
    printKv("Fatal error in incoming thread (control is NULL)");
    return;
  }

  Timespan timeout(1, 250);
  char* buf = NULL;
  printKv("Handling incoming server");

  // Check if we were cancelled every 1.25 seconds
  //client.setReceiveTimeout(timeout);

  while(!isCancelled()) {
    try {
      try {
        KeyValueStore::handleRequest(client, *control, buf);
      } catch(TimeoutException& notUsed(e)) {
        // Just want to check if we were cancelled
        if(buf != NULL)
          delete [] buf;
      }
    } catch(Exception& e) {
      printKv("Exception caught: "<< e.displayText() << ". Terminating incoming server thread.");
      cancel();
    }
  }

  if(buf != NULL)
    delete [] buf;

  printKv("Closing socket to old incoming server");
  client.close();
}

void KeyValueStore::InternalThread::outgoingRoutine()
{
  unsigned currId = -1;
  const Config::ServerInformation& info = config.getServerInformation();
  Config::ThreadControl& control = config.getThreadControl();
  Mutex& outMutex = control.getOutMutex();
  Condition& cond = control.getOutCondition();
  Timespan timeout(3, 0); // 3 seconds to receive response
  bool connected = false;
  unsigned contentLength = 0;
  char* buf = NULL;

  while(control.isLive()) {
    outMutex.lock();
    cond.wait(outMutex);

    // Check if server changed
    if(control.getReconnectedTo()) {
      printKv("Reconnecting");
      currId = control.getConnectedToId();
      const Config::ServerInformation& nextInfo = config.getServerInformation(currId);
      try {
        if(connected)
          sock.close();
        sock.connect(SocketAddress(nextInfo.address, nextInfo.internalPort), Timespan(5, 0));
        connected = true;
        printKv("Connected to outgoing server: "<< nextInfo.id);
        //sock.setReceiveTimeout(timeout);
        KeyValueStore::replicateDB(sock, control.getMachineCount());
      } catch(Exception& e) {
        printKv("Could not connect to next key-value internal server ("<< e.displayText() <<")" << endl
          << "System may be inconsistent until next reconnect.");
        connected = false;
      }
    }

    while(!forwardQueue.isEmpty()) {
      pair<char, string> req(forwardQueue.dequeue());
      if(req.first == Protocol::REPL_INS || req.first == Protocol::REPL_REM) {
        try {
          KeyValueStore::sendRequest(sock, req);
        } catch(Exception& e) {
          printKv("Could not replicate: "<< e.displayText());
        }
      } else if(req.first == Protocol::INS_MOVIE) { // Requests without responses
        KeyValueStore::sendRequest(sock, req); // Not waiting for a response
      } else if(req.first == Protocol::LUP_MOVIE && false) { // This returns many responses need to get all
        printKv("Received movie lookup forward");
        KeyValueStore::sendRequest(sock, req);
        
        pair<char, string> resp(KeyValueStore::rcvResponse(sock, buf));
        while(resp.first != Protocol::MLOOKUP_DONE && resp.first != Protocol::LOOKUP_NO) {
          resp = KeyValueStore::rcvResponse(sock, buf);
          respBuffer.enqueue(resp);
//          replyForward(); // Send as we get them
        }
        respBuffer.enqueue(resp); // Make sure we also send our done response

        printKv("Served movies");

        replyForward();
      } else {
        // Forward and wait for response.
        try {
          KeyValueStore::sendRequest(sock, req);

          pair<char, string> resp(KeyValueStore::rcvResponse(sock, buf));
          respBuffer.enqueue(resp);

          replyForward();
        } catch(Exception& e) {
          // If we timeout or anything, treat request as failed
          printKv("Exception while forwarding: "<< e.displayText());
          respBuffer.enqueue(pair<char, string>(Protocol::REQ_FAIL, ""));
          replyForward();
        }
      }
    }
 //   if(!respBuffer.isEmpty())
   //   replyForward();

    outMutex.unlock();
  }
}

void KeyValueStore::InternalThread::replyForward()
{
  procForward.lock();
  forwardCond.signal();
  procForward.unlock();
}

