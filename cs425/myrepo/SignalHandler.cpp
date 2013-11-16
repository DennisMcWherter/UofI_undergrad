/**
 * SignalHandler.cpp
 *
 * Signal handler implementation
 * Author: Dennis J. McWherter, Jr.
 */

#include <csignal>
#include <iostream>

#include "SignalHandler.h"
#include "Config/Config.h"

Config* SignalHandler::config = NULL;

using namespace std;

SignalHandler::SignalHandler(Config& config)
{
  SignalHandler::config = &config;
}

SignalHandler::~SignalHandler()
{
}

void SignalHandler::install()
{
  signal(SIGINT, SignalHandler::signalCallback);
}

void SignalHandler::signalCallback(int signal)
{
  if(signal != SIGINT || SignalHandler::config == NULL)
    return; // Ignore
  cout<< "Shutting down threads..." << endl;
  SignalHandler::config->getThreadControl().shutdown();
}
