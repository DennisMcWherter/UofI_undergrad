/**
 * src/main.cpp
 * Main file execution
 *
 * Author: Dennis J. McWherter, Jr.
 * 19 January 2012
 */

#include <cstring>
#include <iostream>
#include <string>

#include "../include/Encryptor.h"

using namespace std;

/**
 * Main Entry Point
 */
int main(int argc, char** argv)
{
  string inFile, outFile, key, tmpInput;
  bool decrypt = false;

  if(argc == 4 || argc == 5) {
    inFile  = argv[argc-3];
    outFile = argv[argc-2];
    key     = argv[argc-1];
    if(argc == 5) {
      decrypt = true;
    }
  } else if(argc > 1) {
    if(!strncmp(argv[1], "--help", strlen(argv[1]))) {
      cout<< argv[0] << " usage: [-d] <inputFile> <outputFile> <key>" << endl;
    }
  } else {
    cout<< "Encrypt or Decrypt [e/d]: ";
    cin>> tmpInput;
    if(tmpInput.compare("d") == 0) {
      cout<< "Program set to decrypt message...\n";
      decrypt = true;
    } else {
      cout<< "Program set to encrypt message...\n";
    }
    cout<< "Please enter input file: ";
    cin >> inFile;
    cout<< "Please enter output file: ";
    cin >> outFile;
    cout<< "Please enter encryption key: ";
    cin >> key;
  }

  try {
    Encryptor encrypt(inFile.c_str(), outFile.c_str(), key.c_str());

    if(decrypt)
      encrypt.decrypt();
    else
      encrypt.encrypt();
  } catch ( const char* e ) {
    cout<< "Exception Caught: " << e << endl;
  }

  cout<< "File: " << inFile << ((decrypt) ? " decrypted to " : " encrypted to ") << outFile << " successfully!\n";

  return 0;
}
