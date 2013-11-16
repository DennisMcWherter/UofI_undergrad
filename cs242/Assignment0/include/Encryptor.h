/**
 * includes/Encryptor.h
 *
 * Encapsulation of all encryption methods into a single
 *  object
 *
 *  Author: Dennis J. McWherter, Jr.
 *  19 January 2012
 */

#ifndef _ENCRYPTOR_H__
#define _ENCRYPTOR_H__

#include <fstream>
#include <string>

class Encryptor
{
public:
  Encryptor(const char* inFile, const char* outFile="encrypted.bin", const char* key="defaultKey");
  virtual ~Encryptor();

  // Encryption functions
  int encrypt();
  int decrypt();

  // Get functions
  void setInFile(const char* inFile);
  void setOutFile(const char* outFile);

  // Set functions
  const char* getInFile() const;
  const char* getOutFile() const;

private:
  // Private helper
  void encryptData(std::ifstream& inStream, std::ostream& outStream) const;
  void decryptData(std::ifstream& inStream, std::ostream& outStream) const;

  const char* _inFile;
  const char* _outFile;
  std::string _key;
};

#endif /* _ENCRYPTOR_H__ */
