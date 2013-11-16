/**
 * ConfigHandler.cpp
 *
 * Configuration handler
 * Author: Dennis J. McWherter, Jr.
 */

#include "ConfigHandler.h"
#include "ConfigReader.h"

#include "Poco/Exception.h"

using Poco::XML::Attributes;
using Poco::XML::ContentHandler;
using Poco::XML::Locator;
using Poco::XML::XMLChar;
using Poco::XML::XMLReader;
using Poco::XML::XMLString;

#include <iostream>
using namespace std;

ConfigHandler::ConfigHandler(unsigned id)
  : locator(NULL), config(id)
{
}

ConfigHandler::~ConfigHandler()
{
}

void ConfigHandler::startDocument()
{
  cout << "Parsing configuration... ";
}

void ConfigHandler::endDocument()
{
  cout << "Done" << endl;
}

void ConfigHandler::setDocumentLocator(const Locator* loc)
{
  locator = loc;
}

void ConfigHandler::startElement(const XMLString& uri, const XMLString& localName, const XMLString& qname, const Attributes& attrList)
{
  if(localName.compare("ServerList") == 0) { // Root element
    if(attrList.getLength() != 1 || attrList.getLocalName(0).compare("numKeys") != 0)
      throw Poco::Exception("Invalid format for root element");
    config.setNumberOfKeys(atoi(attrList.getValue(0).c_str()));
    return;
  }

  // Parse data
  Config::ServerInformation info;

  if(attrList.getLength() != 5)
    throw Poco::Exception("Invalid configuration line detected");

  for(int i = 0 ; i < attrList.getLength() ; ++i) {
    const XMLString& name = attrList.getLocalName(i);
    const XMLString& val  = attrList.getValue(i);

    if(name.compare("id") == 0)
      info.id = atoi(val.c_str());
    else if(name.compare("bindAddr") == 0)
      info.address = val;
    else if(name.compare("pubPort") == 0)
      info.pubPort = atoi(val.c_str());
    else if(name.compare("internalPort") == 0)
      info.internalPort = atoi(val.c_str());
    else if(name.compare("fdPort") == 0)
      info.fdPort = atoi(val.c_str());
    else
      throw Poco::Exception("Unexpected argument in server line.");
  }

  config.addServer(info);
}

Config& ConfigHandler::getConfiguration()
{
  return config;
}

const Config& ConfigHandler::getConfiguration() const
{
  return config;
}

/* Empty stubs - can implement them later if necessary */
void ConfigHandler::endElement(const XMLString& uri, const XMLString& localName, const XMLString& qname)
{
}

void ConfigHandler::characters(const XMLChar ch[], int start, int length)
{
}

void ConfigHandler::ignorableWhitespace(const XMLChar ch[], int start, int length)
{
}

void ConfigHandler::processingInstruction(const XMLString& target, const XMLString& data)
{
}

void ConfigHandler::startPrefixMapping(const XMLString& prefix, const XMLString& uri)
{
}

void ConfigHandler::endPrefixMapping(const XMLString& prefix)
{
}

void ConfigHandler::skippedEntity(const XMLString& name)
{
}

