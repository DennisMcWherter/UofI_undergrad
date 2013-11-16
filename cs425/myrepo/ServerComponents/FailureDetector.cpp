/**
 * FailureDetector.cpp
 *
 * Failure detection implementation
 * Author: Dennis J. McWherter, Jr.
 */

#include "FailureDetector.h"
#include "Protocol.h"
#include "../ConcurrentQueue.hpp"
#include "../Config/Config.h"

#include <iostream>
#include <string>
#include <utility>

// POCO
#include "Poco/Condition.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/Timespan.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"

using Poco::Condition;
using Poco::Exception;
using Poco::Mutex;
using Poco::Thread;
using Poco::Timespan;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Net::StreamSocket;

using namespace std;

#define printFd(x) (cout << "FDServer: " << x << endl)

ConcurrentQueue<pair<unsigned char, unsigned> > fwdQueue;
ConcurrentQueue<bool> requested;
Mutex fdMutex;
Mutex pingMutex;
Condition pingCond;
bool pingFailed;
bool pongFailed;

FailureDetector::FailureDetector(Config& config)
  : config(config)
{
  pingFailed = false;
  pongFailed = false;
}

FailureDetector::~FailureDetector()
{
}

/** Packet methods */
unsigned FailureDetector::joinPacket(unsigned char tag, unsigned char mCount, unsigned char id)
{
  unsigned ret = id;
  ret |= ((mCount & 0xff) << 8);
  ret |= ((tag & 0xff) << 16);
  return ret;
}

unsigned FailureDetector::unpackTag(unsigned packet)
{
  return ((packet & (0xff << 16)) >> 16);
}

unsigned FailureDetector::unpackMCount(unsigned packet)
{
  return ((packet & (0xff << 8)) >> 8);
}

unsigned FailureDetector::unpackId(unsigned packet)
{
  return (packet & 0xff);
}

void FailureDetector::run()
{
  Thread pingThread;
  Thread pongThread;
  StreamSocket placeHolder;

  PingThread ping(config);
  pingThread.start(ping);

  // TODO: Change this from being static!
 // config.getThreadControl().setPublicId(config.getInternalId());

  try {
    // Place hold are first
    PongThread currThread(config, placeHolder);

    // Handle join requests
    waitForIncoming(currThread, pongThread);
  } catch(Exception& e) {
    printFd("FD exception (unhandled): "<< e.displayText());
  }

  printFd("Shutting down");

  pingThread.join();
  pongThread.join();
}

void FailureDetector::waitForIncoming(PongThread& currThread, Poco::Thread& pongThread)
{
  Config::ThreadControl& control = config.getThreadControl();
  int newId = 0;
  const Config::ServerInformation& info = config.getServerInformation();
  SocketAddress sock(info.address, info.fdPort);
  ServerSocket server;
  StreamSocket conn;
  static bool first = true;

  // Set reuse address - we don't like connection issues
  server.bind(sock, true);
  server.listen(10);

  while(control.isLive()) {
    newId = doInit(server, conn);
    
    if(newId == -1)
      continue; // Error

    //printFd("Tag: "<< tag << " | MCount: "<< mCount << " | NewID: "<< newId);

    printFd("*** Signaling join");
    currThread.signalJoin(newId);
    printFd("*** Thread signled to join...");

    printFd("*** Joining old thread");
    if(!first)
      pongThread.join();
    printFd("*** Old thread joined ***");

    first = false;
    
    control.setConnectedId(newId);
    currThread = PongThread(config, conn);
    pongThread.start(currThread);

    // Signal ping thread just in case he is waiting
    pingMutex.lock();
    pingCond.signal();
    pingMutex.unlock();

    printFd("Done Ping ***");

    // Notify KV thread
    control.getInMutex().lock();
    control.setConnectedId(newId);
    control.getInCondition().signal();
    control.getInMutex().unlock();

    printFd("Done Control ***");
  }
}

int FailureDetector::doInit(ServerSocket& server, StreamSocket& conn)
{
  Timespan timeout(3, 0); // 3 seconds
  unsigned id = -1;
  char recv;
  static const char suc = Protocol::REQ_SUC;
  static const char bad = Protocol::REQ_FAIL;

  // Make sure this is a valid connection
  try {
    printFd("PONG: *** Waiting for new connection");
    conn = server.acceptConnection();
    conn.setReceiveTimeout(timeout);
    conn.receiveBytes(&recv, sizeof(recv));
    printFd("PONG: Received new connection");
    if(recv != Protocol::JOIN && recv != Protocol::FDCON) { // Received wrong request type
      printFd("No proper join received ("<< hex << (unsigned)recv<< ")");
      return -1;
    } else if(recv == Protocol::JOIN) {
      printFd("PONG: *** JOIN request received");
      conn.receiveBytes(&id, sizeof(id));
    } else if(recv == Protocol::FDCON) {
      printFd("PONG: *** Reconnect request received");
      conn.receiveBytes(&id, sizeof(id));

      fdMutex.lock();
      bool failed = pongFailed;
      fdMutex.unlock();

      if(failed) {
        conn.sendBytes(&suc, sizeof(suc));
        printFd("PONG: Accepting reconnect");
      } else {
        conn.sendBytes(&bad, sizeof(bad));
        printFd("PONG: Did not detect failure, rejecting reconnect request");
        return -1;
      }
    }
  } catch( ... ) {
    printFd("No proper join received");
    return -1;
  }

  printFd("PONG: Handling new connection");

  return id;
}

/** Pong Thread */
void FailureDetector::PongThread::run()
{
  try {
    handleClient();
  } catch(Exception& e) {
    printFd("PONG (unhandled exception): "<< e.displayText());
  }
}

void FailureDetector::PongThread::handleClient()
{
  string addr = client.peerAddress().host().toString();
  Poco::UInt16 port = client.address().port();

  printFd("PONG: Server connected to failure detector ("<< addr << ":" << port << ")");

  try {
    handleRequests();
  } catch(Exception& e) {
    printFd("PONG: Detected failure (" << addr << " - " << e.displayText() << ")");
    fdMutex.lock();
    pongFailed = true;
    fdMutex.unlock();
  }

  printFd("PONG: Thread terminating...");

  client.close();
}

void FailureDetector::PongThread::handleRequests()
{
  Timespan timeout(Poco::Timespan::TimeDiff(5000000)); // 10 second timeout
  char buf;
  const static char pong  = Protocol::PONG;
  const static char rconn = Protocol::RCONN;
  bool shutdownThread = false;
  unsigned rconId = 1;
  unsigned internalId = config->getInternalId();
    
  fdMutex.lock();
  pongFailed = false;
  fdMutex.unlock();

  client.setReceiveTimeout(timeout);

  int r = 0;
  while((r = client.receiveBytes(&buf, sizeof(buf))) && !shutdownThread) {
    printFd("Received byte: "<< r << hex <<" (0x"<< (unsigned)buf <<")");
    if(r < 1) {
      printFd("It seems the connection has closed.");
      throw Exception("Socket closed");
    }
    r = 0; // reset

    if(buf == Protocol::EXIT) {
      printFd("PONG: Server quit");
      break; // Quit
    } else if(buf == Protocol::NEWID) {
      handleNewId(internalId);
    } else if(buf == Protocol::SETMC) {
      handleSetMC(internalId);
    } else if(buf == Protocol::PING) { // Send pong
      client.sendBytes(&pong, sizeof(pong));
    }

    mutex.lock();
    shutdownThread = joinReq;
    rconId = nextId;
    mutex.unlock();
  }

  if(shutdownThread) { // Let client know to connect to new guy
    try {
      printFd("PONG: JOIN detected - forwarding id to old client");
      client.sendBytes(&rconn, sizeof(rconn));
      client.sendBytes(&rconId, sizeof(rconId));
      printFd("*** Connecting to: "<< rconId);
      fwdQueue.clear(); // New set of incoming messages - discard these
      requested.clear(); // Clear this
    } catch(Exception& e ) {
      printFd("Could not tell next server to forward: "<< e.displayText());
    }
  } else if(r < 1) {
    throw Exception("Socket closed");
  }
}

void FailureDetector::PongThread::handleNewId(unsigned internalId)
{
  unsigned packet = 0;

  client.receiveBytes(&packet, sizeof(packet));

  // Unpack the packet
  unsigned tag    = FailureDetector::unpackTag(packet);
  unsigned mCount = FailureDetector::unpackMCount(packet);
  unsigned newId  = FailureDetector::unpackId(packet);

  // Forward the packet again
  if(tag > internalId && !requested.isEmpty()) {
    printFd("Packet died here (tag: "<< tag<<") - my request is better (empty: " << requested.isEmpty()<<")");
  } else if(tag != internalId) {
    unsigned pack = FailureDetector::joinPacket(tag, mCount + 1, newId + 1);
    pair<unsigned char, unsigned> req(Protocol::NEWID, pack);
    requested.clear();
    fwdQueue.enqueue(req);
  } else {
    printFd("Final = Tag: "<< tag <<" | MCount: "<< mCount <<" | NewID: "<< newId);
    // Now to send the new packet - it has been decided that I am 0!
    unsigned pack = FailureDetector::joinPacket(tag, mCount, 0);
    pair<unsigned char, unsigned> req(Protocol::SETMC, pack);
    config->getThreadControl().setPublicId(0);
    fwdQueue.enqueue(req);
    requested.clear(); // No longer requested
  }
}

void FailureDetector::PongThread::handleSetMC(unsigned internalId)
{
  unsigned packet = 0;
  client.receiveBytes(&packet, sizeof(packet));

  // Unpack the packet
  unsigned tag    = FailureDetector::unpackTag(packet);
  unsigned mCount = FailureDetector::unpackMCount(packet);
  unsigned newId  = FailureDetector::unpackId(packet);

  newId += 1;

  if(tag != internalId) {
    config->getThreadControl().setPublicId(newId);
    unsigned pack = FailureDetector::joinPacket(tag, mCount, newId);
    pair<unsigned char, unsigned> req(Protocol::SETMC, pack);
    fwdQueue.enqueue(req);
  } else { // Paranoia
    config->getThreadControl().setPublicId(0);
  }

  requested.clear(); // Make sure this is cleared

  config->getThreadControl().setMachineCount(mCount);

  printFd("My id: "<< config->getThreadControl().getPublicId()<< " (ServerCount: "<< config->getThreadControl().getMachineCount()<< ")");
}

void FailureDetector::PongThread::signalJoin(unsigned newId)
{
  mutex.lock();
  joinReq = true;
  nextId = newId;
  mutex.unlock();
}

/** Ping Thread */
void FailureDetector::PingThread::run()
{
  Config::ThreadControl& control = config.getThreadControl();
  int fail = 0;
  static bool first = true;

  printFd("PING: Trying to connect to a server in the list");
  while(control.isLive()) {
    if(first && fail == 3) { // MAGIC Number for assignment (4 servers maximum)
      printFd("PING: Waiting for server to connect");
      config.getThreadControl().setMachineCount(1);
      pingMutex.lock();
      pingCond.wait(pingMutex);
      pingMutex.unlock();
      printFd("PING: Server connected!");
    }
    const Config::ServerInformation& server = (nextId == -1) ? config.getNextServer() : config.getServerInformation(nextId);
    //printFd("NextID: "<< nextId);
    nextId = -1;

    if(!tryConnect(server)) {
      fail++;
      continue;
    }

    printFd("Fail: "<< fail);

    fail = 0;
    first = false;

    // Notify KV thread
    control.getOutMutex().lock();
    control.setConnectedToId(server.id);
    control.setReconnectedTo();
    control.getOutCondition().signal();
    control.getOutMutex().unlock();

    try {
      detectFailures();
    } catch(Exception& e) {
      printFd("PING exception (unhandled): "<< e.displayText());
    }
  }
}

bool FailureDetector::PingThread::tryConnect(const Config::ServerInformation& server)
{
  Timespan timeout(Timespan::TimeDiff(2000000)); // 2 second timeout
  SocketAddress newSock(server.address, server.fdPort);
  printFd("PING: Trying server: "<< server.address << ":" << server.fdPort<< "(id: "<<
    server.id << ")");
  try {
    serverSock.connect(newSock, timeout);
  } catch( ... ) {
    Thread::sleep(250);
    return false;
  }
  return true;
}

void FailureDetector::PingThread::detectFailures()
{
  Timespan timeout(Timespan::TimeDiff(10000000)); // 10 second timeout
  string addr = serverSock.peerAddress().toString();
  static const char join = Protocol::JOIN;
  static const char newid = Protocol::NEWID;
  static const char fdcon = Protocol::FDCON;
  unsigned internalId = config.getInternalId();
  char resp;
  static bool first = true;

  serverSock.setReceiveTimeout(timeout);
  requested.clear(); // Only 1 request at a time gentlemen

  fdMutex.lock();
  bool failed = pingFailed;
  fdMutex.unlock();

  unsigned packet = FailureDetector::joinPacket(internalId, 1, 0);
  
  if(config.getThreadControl().getMachineCount() != 1 && !failed) {
    printFd("Sending join request...");
    // Send join request
    serverSock.sendBytes(&join, sizeof(join));
    serverSock.sendBytes(&internalId, sizeof(internalId));
    if(first) {
      serverSock.sendBytes(&newid, sizeof(newid));
      serverSock.sendBytes(&packet, sizeof(packet));
      requested.enqueue(true); // Using queue for simplicity - basically just say we sent newid request.
      first = false;
    }
  } else if(failed) {
    printFd("Sending reconnect request");
    serverSock.sendBytes(&fdcon, sizeof(fdcon));
    serverSock.sendBytes(&internalId, sizeof(internalId));
    serverSock.receiveBytes(&resp, sizeof(resp));
    if(resp != Protocol::REQ_SUC) {
      printFd("Connected server did not detect failure... Trying another");
      serverSock.close();
      return; // Did not work
    }
    // Initiate new id request if successful
    serverSock.sendBytes(&newid, sizeof(newid));
    serverSock.sendBytes(&packet, sizeof(packet));
  } else {
    serverSock.sendBytes(&join, sizeof(join));
    serverSock.sendBytes(&internalId, sizeof(internalId));
    printFd("Machine count: "<< config.getThreadControl().getMachineCount());
    first = false;
  }

  printFd("PING: Connected to failure detector ("<< addr << ") - server id "<< config.getRecentServerId());

  try {
    doPing();
  } catch(Exception& e) {
    printFd("PING: Detected failure (" << addr << " - " << e.displayText() << ")");
    fdMutex.lock();
    pingFailed = true;
    fdMutex.unlock();
  }

  serverSock.close();
}

void FailureDetector::PingThread::doPing()
{
  int next = -1;
  char msg  = Protocol::PING;
  char recv;

  fdMutex.lock();
  pingFailed = false;
  fdMutex.unlock();

  while(serverSock.sendBytes(&msg, sizeof(msg))) {
    if(serverSock.receiveBytes(&recv, sizeof(recv)) < 1) {
      printFd("PING: Problem receiving from PONG server");
    }
    if(recv == Protocol::RCONN) {
      printFd("PING: Told to reconnect (waiting for more information)");
      serverSock.setReceiveTimeout(Timespan(3, 0)); // 3 seconds to get next id
      serverSock.receiveBytes(&next, sizeof(next));
      printFd("PING: next server: "<< next);
      nextId = next; // TODO: Test further. Maybe clear a buffer or something?
      break;
    } else if(recv != Protocol::PONG) {
      throw Exception("Invalid message received");
    } else if(!config.getThreadControl().isLive()) {
      printFd("Shutting down PING thread");
      break;
    }
    doForward(); // Does not wait for response
    Thread::sleep(3000);
  }
}

void FailureDetector::PingThread::doForward()
{
  while(!fwdQueue.isEmpty()) {
    printFd("Forwarded message");
    pair<unsigned char, unsigned> req(fwdQueue.dequeue());
    unsigned char proto = req.first;
    unsigned packet = req.second;
    serverSock.sendBytes(&proto, sizeof(proto));
    serverSock.sendBytes(&packet, sizeof(packet));
  }
}

void FailureDetector::PingThread::setNextId(int id)
{
  pMutex.lock();
  nextId = id;
  pMutex.unlock();
}

int FailureDetector::PingThread::getNextId()
{
  pMutex.lock();
  int id = nextId;
  pMutex.unlock();
  return nextId;
}

