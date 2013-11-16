/**
 * src/Encryptor.cpp
 *
 * Implementation of encryptor
 *
 * Author: Dennis J. McWherter, Jr.
 * 19 January 2012
 */

#include "../include/Encryptor.h"

using namespace std;

/**
 * Constructor
 * Initializes the encryptor
 */
Encryptor::Encryptor(const char* inFile, const char* outFile, const char* key)
  : _inFile(inFile), _outFile(outFile), _key(key)
{
  if(inFile == NULL || outFile == NULL || key == NULL)
    throw "Could not initialize Encryptor: NULL values detected\n";
}

/**
 * Destructor
 * Basic clean up
 */
Encryptor::~Encryptor()
{
}

/**
 * setInFile
 * @brief Change the input file for the encryptor to encrypt
 *
 * @param const char* inFile - New input filename
 *
 * @return void
 */
void Encryptor::setInFile(const char* inFile)
{
  _inFile = inFile;
}

/**
 * setOutFile
 * @brief Change the output file where
 *
 * @param const char* outFile - New output filename
 *
 * @return void
 */
void Encryptor::setOutFile(const char* outFile)
{
  _outFile = outFile;
}

/**
 * getInFile
 * @brief get the name of the current input file
 *
 * @return const char*
 */
const char* Encryptor::getInFile() const
{
  return _inFile;
}

 /**
  * getOutFile
  * @brief get the name of the current output file
  *
  * @return const char*
  */
const char* Encryptor::getOutFile() const
{
  return _outFile;
}

/**
 * encrypt
 * @brief Encrypt data
 *
 * @return int ; not not 0 = failure, 0 = success
 */
int Encryptor::encrypt()
{
  std::ifstream inStream;
  std::ofstream outStream;

  // Reading input
  try {
    inStream.open(_inFile, fstream::in | fstream::binary);
  } catch ( ... ) {
    return 0x01; // If 1 then failed to read input
  }

  // Open output file
  try {
    outStream.open(_outFile, ostream::out | ostream::binary);
  } catch ( ... ) {
    inStream.close();
    return 0x02; // If 2 then failed to open output
  }

  encryptData(inStream, outStream);

  inStream.close();
  outStream.close();

  return 0;
}

/**
 * decrypt
 * @brief Decrypt input data
 *
 * @return int ; not not 0 = failure, 0 = success
 */
int Encryptor::decrypt()
{
  std::ifstream inStream;
  std::ofstream outStream;

  // Reading input
  try {
    inStream.open(_inFile, fstream::in | fstream::binary);
  } catch ( ... ) {
    return 0x01; // If 1 then failed to read input
  }

  // Open output file
  try {
    outStream.open(_outFile, ostream::out | ostream::binary);
  } catch ( ... ) {
    inStream.close();
    return 0x02; // If 2 then failed to open output
  }

  decryptData(inStream, outStream);

  inStream.close();
  outStream.close();

  return 0;
}

/**
 * encryptData (private helper)
 * @brief Actually encrypt the data
 *
 * @param inStream - valid input stream to encrypt
 * @param outStream - valid output stream to write to
 *
 * @return void
 */
void Encryptor::encryptData(ifstream& inStream, ostream& outStream) const
{
  string line;
  int lineLength = 0;
  char buffer[4]; // Convert line length to char

  if(!inStream.is_open())
    return;

  while(!inStream.eof()) {
    getline(inStream, line);

    lineLength = line.length();

    for(unsigned i = 0 ; i < line.length() ; ++i) {
      if(i%2)
        line[i] = line[i] ^ (_key[i % _key.length()] + lineLength);
      else
        line[i] = line[i] ^ (_key[i % _key.length()] - lineLength);
    }

    // Pack the length bits in the first 4 bytes
    if(lineLength != 0) {
      buffer[0] = lineLength & 0xff;
      buffer[1] = lineLength & (0xff << 8);
      buffer[2] = lineLength & (0xff << 16);
      buffer[3] = lineLength & (0xff << 24);
    } else { // If blank line, preserve it
      buffer[0] = 0x02;
      buffer[1] = buffer[2] = buffer[3] = 0x00;
      line += '\n' ^ (_key[0] - 2);
      line += '\n' ^ (_key[1 % _key.size()] + 2);
    }

    outStream.write(buffer, 4);
    outStream.write(line.c_str(), line.size());
    line.clear();
  } // End of encryption loop

  outStream.write("\n", 1); // Make sure we have a new line to read
}

/**
 * decryptData (private helper)
 * @brief Actually decrypt the data
 *
 * @param inStream - valid input stream to encrypt
 * @param outStream - valid output stream to write to
 *
 * @return void
 */
void Encryptor::decryptData(ifstream& inStream, ostream& outStream) const
{
  string line, decryptedLine;
  int lineLength = 0, currLineLength = 0;
  bool newLine = true;

  if(!inStream.is_open())
    return;

  while(!inStream.eof()) {
    getline(inStream, line);

    if(line.empty())
      continue;

    // Extract lineLength
    if(newLine) {
      lineLength = *((int*)line.c_str());
      currLineLength = 0;
      line = line.substr(4); // Eat lineLength bytes
      newLine = false;
    }

    for(unsigned i = 0 ; i < line.length() ; ++i, ++currLineLength) {

      if(currLineLength == lineLength) { // Process new line
        line = line.substr(lineLength);
        lineLength = *((int*)line.c_str());
        i = currLineLength = -1;
        line = line.substr(4); // Eat lineLength bytes
        continue;
      }

      if(i%2)
        decryptedLine += line[i] ^ (_key[i % _key.length()] + lineLength);
      else
        decryptedLine += line[i] ^ (_key[i % _key.length()] - lineLength);
    }

    if(currLineLength == lineLength)
      newLine = true;

    outStream.write(decryptedLine.c_str(), decryptedLine.size());
    decryptedLine.clear();
  } // End of decrypt loop
}
