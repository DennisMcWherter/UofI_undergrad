/**
 * main.cpp
 *
 * Program main entry point
 * Author: Dennis J. McWherter, Jr.
 */

#include <iostream>

#include "Server.h"
#include "SignalHandler.h"
#include "Config/ConfigReader.h"

#include "Poco/Exception.h"

using namespace std;

int main(int argc, char* argv[])
{
  const char* addr = NULL;
  unsigned id = 0;
  if(argc < 3) {
    cerr << "Usage: "<< argv[0] << " <config_file> <server_id>" << endl;
    //return 1;
    // DEBUG:
    addr = "sample.xml";
  } else {
    addr = argv[1];
    id   = atoi(argv[2]);
  }

  ConfigReader config(addr, id); // Temporarily hardcoded

  cout << "Initializing MyRepo server" << endl;
  
  if(!config.parse()) {
    cerr << "Aborting" << endl;
    cin.get(); // Temp hack
    return 1;
  }

  Server server(config.getConfiguration());

  cout << "Server initialized" << endl;

#if 0 // Need more time to actually set this up properply
  SignalHandler sigh(config.getConfiguration()); // Simply initialize
  sigh.install();
#endif

  try {
    server.start();
  } catch(Poco::Exception& e) {
    cout<< "Uncaught exception: "<< e.displayText() << endl;
  }

  cout << "Exiting" << endl;
  cin.get(); // Temp hack
  return 0;
}
