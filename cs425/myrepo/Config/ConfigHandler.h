/**
 * ConfigHandler.h
 *
 * Configuration handler
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef CONFIG_CONFIGHANDLER_H__
#define CONFIG_CONFIGHANDLER_H__

#include "Config.h"

#include "Poco/SAX/Attributes.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/SAX/Locator.h"
#include "Poco/SAX/XMLReader.h"

class ConfigReader;

/**
 * ConfigHandler
 *
 * Configuration handler for parser callbacks
 */
class ConfigHandler : public Poco::XML::ContentHandler
{
public:
  /**
   * Constructor
   *
   * @param id    Current server id
   */
  ConfigHandler(unsigned id);

  /**
   * Destructor
   */
  virtual ~ConfigHandler();

  /** Content handler section */
  /**
   * Set document locator
   *
   * @param loc   New locator
   */
  virtual void setDocumentLocator(const Poco::XML::Locator* loc);

  /**
   * Start document
   */
  virtual void startDocument();

  /**
   * End document
   */
  virtual void endDocument();

  /**
   * Start element
   *
   * @param uri
   * @param localName
   * @param qname
   * @param attrList
   */
  virtual void startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attrList);

  /**
   * End element
   *
   * @param uri
   * @param localName
   * @param qname
   */
  virtual void endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname);

  /**
   * Characters
   *
   * @param ch   Character
   * @param start
   * @param length
   */
  virtual void characters(const Poco::XML::XMLChar ch[], int start, int length);
	
  /**
   * Ignorable whitespace
   *
   * @param ch
   * @param start
   * @param length
   */
	virtual void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length);
	
  /**
   * Processing instruction
   *
   * @param target
   * @param data
   */
  virtual void processingInstruction(const Poco::XML::XMLString& target, const Poco::XML::XMLString& data);
	
  /**
   * Start prefix mapping
   *
   * @param prefix
   * @param uri
   */
	virtual void startPrefixMapping(const Poco::XML::XMLString& prefix, const Poco::XML::XMLString& uri);
	
  /**
   * End prefix mapping
   *
   * @param prefix
   */
	virtual void endPrefixMapping(const Poco::XML::XMLString& prefix);
	
  /**
   * Skipped entry
   *
   * @param name
   */
	virtual void skippedEntity(const Poco::XML::XMLString& name);

  /**
   * Get the configuration object
   *
   * @return  Reference to configuration structure
   */
  virtual Config& getConfiguration();

  /**
   * Get the configuration object
   *
   * @return  Reference to configuration structure. const-safe version.
   */
  virtual const Config& getConfiguration() const;

private:
  const Poco::XML::Locator* locator;
  Config config;
};

#endif /** CONFIG_CONFIGHANDLER_H__ */
