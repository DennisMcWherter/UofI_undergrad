/**
 * ConfigReader.cpp
 *
 * Configuration reader
 * Author: Dennis J. McWherter, Jr.
 */

// Project
#include "ConfigReader.h"

#include <iostream>

// POCO
#include "Poco/Exception.h"
#include "Poco/SAX/SAXParser.h"

using Poco::XML::SAXParser;
using Poco::XML::XMLReader;
using Poco::XML::XMLString;

using std::cerr;
using std::endl;
using std::string;

ConfigReader::ConfigReader(string file, unsigned id)
  : fileName(file), handler(id)
{
  parser.setContentHandler(&handler);
}

ConfigReader::~ConfigReader()
{
}

bool ConfigReader::parse()
{
  try {
    parser.parse(fileName.c_str());
  } catch(Poco::Exception& e) {
    cerr << e.displayText() << endl;
    return false;
  }
  return true;
}

Config& ConfigReader::getConfiguration()
{
  return handler.getConfiguration();
}

const Config& ConfigReader::getConfiguration() const
{
  return handler.getConfiguration();
}
