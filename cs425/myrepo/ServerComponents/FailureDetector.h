/**
 * FailureDetector.h
 *
 * Failure Detector interface
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef SERVERCOMPONENTS_FAILUREDETECTOR_H__
#define SERVERCOMPONENTS_FAILUREDETECTOR_H__

#include "Poco/Mutex.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"

#include "../Config/Config.h"

/**
 * Failure detection module
 */
class FailureDetector : public Poco::Runnable
{
public:
  /**
   * Constructor
   */
  FailureDetector(Config& config);

  /**
   * Destructor
   */
  virtual ~FailureDetector();

  /**
   * Overloaded entry point for thread
   */
  virtual void run();

private:
  /** NOTE: Due to packet limitations, the system only supports up to 256 machines */
  /**
   * Create a join packet
   *
   * @param tag     The internal id of the sending process
   * @param mcount  The number of machines counted up to this point
   * @param id      The current process' public id
   * @return  A packet with this information above
   */
  static unsigned joinPacket(unsigned char tag, unsigned char mCount, unsigned char id);

  /**
   * Extract the tag from a packet
   *
   * @param packet    The packet
   * @return  The tag on the packet
   */
  static unsigned unpackTag(unsigned packet);

  /**
   * Extract the machine count from a packet
   *
   * @param packet    The packet
   * @return  The macine count on the packet
   */
  static unsigned unpackMCount(unsigned packet);

  /**
   * Extract the previous machine id from a packet
   *
   * @param packet    The packet
   * @return  The id on the packet
   */
  static unsigned unpackId(unsigned packet);

  /**
   * Thread for sending detection messages
   */
  class PingThread : public Poco::Runnable
  {
  public:
    PingThread(Config& config) : config(config), nextId(-1) {}
    virtual void run();
    virtual bool tryConnect(const Config::ServerInformation& server);
    virtual void detectFailures();
    virtual void doPing();
    virtual void doForward();
    virtual void setNextId(int);
    virtual int getNextId();

  private:
    Config& config;
    Poco::Net::StreamSocket serverSock;
    int nextId;
    Poco::Mutex pMutex;
  };

  /**
   * Thread for responding to ping requests
   */
  class PongThread : public Poco::Runnable
  {
  public:
    PongThread(Config& config, const Poco::Net::StreamSocket& sock) : config(&config), client(sock), joinReq(false) {}
    virtual void run();
    virtual void handleClient();
    virtual void handleRequests();
    virtual void handleNewId(unsigned);
    virtual void handleSetMC(unsigned);
    virtual void signalJoin(unsigned id);
    virtual PongThread& operator=(const PongThread& rhs) {
      if(&rhs != this) {
        config = rhs.config;
        client = rhs.client;
        nextId = rhs.nextId;
        joinReq = rhs.joinReq;
      }
      return *this;
    }

  private:
    Config* config;
    Poco::Net::StreamSocket client;
    Poco::Mutex mutex;
    bool joinReq;
    unsigned nextId;
  };

  /**
   * Let servers attempt to connect
   *
   * @param currThread    Reference to the current live thread
   * @param pongThread    Reference to the thread object for the pong thread
   */
  void waitForIncoming(PongThread& currThread, Poco::Thread& pongThread);

  /**
   * Handle initialization
   *
   * @param server    Open server socket
   * @param conn      Client socket to be updated with new connection
   * @return  The internal server id. -1 on error.
   */
  int doInit(Poco::Net::ServerSocket& server, Poco::Net::StreamSocket& conn);

  Config& config;
};

#endif /** SERVERCOMPONENTS_FAILUREDETECTOR_H__ */
