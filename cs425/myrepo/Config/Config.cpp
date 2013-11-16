/**
 * Config.cpp
 *
 * Configuration container implementation
 * Author: Dennis J. McWherter, Jr.
 */

#include "Config.h"

using Poco::Condition;
using Poco::Mutex;

Config::ServerInformation Config::defaultInfo;

Config::Config(unsigned id)
  : id(id), next(id == 0 ? 0 : -1), numKeys(1000000), recent(next)
{
}

Config::~Config()
{
}

const Config::ServerInformation& Config::getServerInformation(unsigned inId) const
{
  std::vector<ServerInformation>::const_iterator it;

  for(it = serverInfo.begin() ; it != serverInfo.end() ; ++it) {
    if(it->id == inId)
      return *it;
  }

  return defaultInfo;
}

const Config::ServerInformation& Config::getNextServer()
{
  return getServerInformation(computeNextServerId());
}

const Config::ServerInformation& Config::getServerInformation() const
{
  return getServerInformation(id);
}

unsigned Config::getInternalId() const
{
  return id;
}

unsigned Config::computeNextServerId()
{
  mutex.lock();
  recent = next;
  next = (next+1) % serverInfo.size();
  if(next == id)
    next = (next+1) % serverInfo.size();
  mutex.unlock();
  return recent;
}

unsigned Config::getNextServerId()
{
  mutex.lock();
  unsigned ret = next;
  mutex.unlock();
  return ret;
}

unsigned Config::getRecentServerId()
{
  mutex.lock();
  unsigned ret = recent;
  mutex.unlock();
  return ret;
}

void Config::addServer(const ServerInformation& info)
{
  serverInfo.push_back(info);
  next = (id + 1) % serverInfo.size();
}

void Config::setNumberOfKeys(unsigned num)
{
  numKeys = num;
}

unsigned Config::getNumberOfKeys() const
{
  return numKeys;
}

Config::ThreadControl& Config::getThreadControl()
{
  return threadControl;
}


/** Thread control */

unsigned Config::ThreadControl::getMachineCount()
{
  mMutex.lock();
  unsigned ret = machineCount;
  mMutex.unlock();
  return machineCount;
}

void Config::ThreadControl::setMachineCount(unsigned count)
{
  mMutex.lock();
  machineCount = count;
  mMutex.unlock();
}

bool Config::ThreadControl::isLive()
{
  sMutex.lock();
  bool ret = !shutdownServer;
  sMutex.unlock();
  return ret;
}

bool Config::ThreadControl::getReconnectedTo()
{
  rMutex.lock();
  bool ret = reconnectedTo;
  reconnectedTo = false;
  rMutex.unlock();
  return ret;
}

void Config::ThreadControl::setReconnectedTo()
{
  rMutex.lock();
  reconnectedTo = true;
  rMutex.unlock();
}

int Config::ThreadControl::getConnectedId()
{
  return connectedId;
}

void Config::ThreadControl::setConnectedId(int id)
{
  connectedId = id;
}

int Config::ThreadControl::getConnectedToId()
{
  return connectedToId;
}

void Config::ThreadControl::setConnectedToId(int id)
{
  connectedToId = id;
}

int Config::ThreadControl::getPublicId()
{
  pMutex.lock();
  int ret = pubId;
  pMutex.unlock();
  return pubId;
}

void Config::ThreadControl::setPublicId(int id)
{
  pMutex.lock();
  pubId = id;
  pMutex.unlock();
}

Condition& Config::ThreadControl::getOutCondition()
{
  return outCond;
}

Condition& Config::ThreadControl::getInCondition()
{
  return inCond;
}

Mutex& Config::ThreadControl::getOutMutex()
{
  return oMutex;
}

Mutex& Config::ThreadControl::getInMutex()
{
  return iMutex;
}

void Config::ThreadControl::shutdown()
{
  sMutex.lock();
  shutdownServer = true;
  sMutex.unlock();
}
