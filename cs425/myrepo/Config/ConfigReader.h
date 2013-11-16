/**
 * ConfigReader.h
 *
 * Configuration Reader interface
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef CONFIG_CONFIGREADER_H__
#define CONFIG_CONFIGREADER_H__

#include "ConfigHandler.h"

#include <string>

#include "Poco/SAX/SAXParser.h"

/**
 * ConfigReader
 *
 * Basic XML configuration reader
 */
class ConfigReader
{
public:
  /**
   * Constructor
   *
   * @param fileName    Configuration file name
   * @param id          Server id
   */
  ConfigReader(std::string fileName, unsigned id);

  /**
   * Destructor
   */
  virtual ~ConfigReader();

  /**
   * Parse a data file
   *
   * @return  true if successful, false otherwise.
   */
  virtual bool parse();

  /**
   * Get configuration structure
   *
   * @return  Reference to configuration structure
   */
  virtual Config& getConfiguration();

  /**
   * Get configuration structure
   *
   * @return  Reference to configuration structure. const-safe version
   */
  virtual const Config& getConfiguration() const;

private:
  std::string fileName;
  Poco::XML::SAXParser parser;
  ConfigHandler handler;
};

#endif /* CONFIG_CONFIGREADER_H__ */
