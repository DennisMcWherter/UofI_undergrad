/**
 * Server.cpp
 *
 * Main server entry point implementation
 * Author: Dennis J. McWherter, Jr.
 */

#include "Config/Config.h"
#include "Server.h"

// Components
#include "ServerComponents/FailureDetector.h"
#include "ServerComponents/KeyValueStore.h"

using Poco::Thread;

Server::Server(Config& config)
  : config(config)
{
}

Server::~Server()
{
}

void Server::start()
{
  FailureDetector fd(config);
  KeyValueStore kv(config);

  // Only failure detection right now
  threads[0].start(fd);
  threads[1].start(kv);

  // The plan:
  // Listen for user input on this thread?

  threads[0].join();
  threads[1].join();
}
