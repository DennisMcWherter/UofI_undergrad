/**
 * Server.h
 *
 * Server interface
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef SERVER_H__
#define SERVER_H__

#include "Poco/Thread.h"

class Config;

class Server
{
public:
  /**
   * Constructor
   *
   * @param config    Configuration object
   */
  Server(Config& config);

  /**
   * Destructor
   */
  virtual ~Server();

  /**
   * Start server
   */
  virtual void start();

private:
  Config& config;
  Poco::Thread threads[3];
};

#endif /** SERVER_H__ */
