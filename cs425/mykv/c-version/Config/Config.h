/**
 * Config.h
 *
 * Configuration information
 */

#ifndef CONFIG_CONFIG_H__
#define CONFIG_CONFIG_H__

#include <pthread.h>

#define CONFIG_ERRMSG_SIZE 128

/**
 * Connection information of 
 *  other internal servers in
 *  the network
 */
typedef struct _connect_config
{
  char* serverIP;
  char* serverSecret;
  unsigned serverPort;
  int sock;
} ConnectionConfig;

/**
 * Configuration options for
 *  the local server
 */
typedef struct _server_config
{
  char* serverSecret;
  char* bindIP;
  unsigned serverId;
  unsigned bindPort;
  unsigned clientPort; /* Port to listen for clients on */
  unsigned numConnections;
  unsigned totalConnections;
  unsigned numServers;
  unsigned numKeys;
  int currentConnection;
  ConnectionConfig* connections;
  pthread_mutex_t mutex;
  int fail_rate; /** DEBUG */
} ServerConfig;

/**
 * Configuration parser state
 *  information
 */
typedef enum _parser_state
{
  VANILLA, /** Nothing has been read, can start anything */
  SERVER, /** Looking for server data */
  CONNECTION, /** Looking for connection data */
  FAILED /** Parser failed */
} ParserState;

/** Configuration methods */
/**
 * Load configuration file
 * NOTE: This method _WILL_ initialize the config object.
 *
 * @param config  Configuration object to store configuration data
 * @param file    File name to be read
 * @return  0 on error else 1.
 */
int loadConfiguration(ServerConfig* config, const char* file);

/**
 * Initialize a server configuration object
 * 
 * @param config    Configuration pointer
 * @return  void
 */
void initServerConfig(ServerConfig* config);

/**
 * Properly clean up a ServerConfig object.
 * NOTE: Behavior of using the object after calling
 *  this method  is undefined.
 * 
 * @param config  Configuration object to destroy
 * @return  void
 */
void destroyServerConfig(ServerConfig* config);

/** Internal Methods */

/**
 * Load configuration file
 *
 * @param config  Configuration object to store configuration data
 * @param file    File name to be read
 * @param line    Line number if reader fails (else disregard)
 * @param msg     Failure message if reader fails (else disregard)
 * @return  0 on error else 1.
 */
int loadConfig(ServerConfig* config, const char* file, int* line, char msg[CONFIG_ERRMSG_SIZE]);

/**
 * Parse a line
 *
 * @param config  Server configuration object to populate
 * @param line    The line to parse
 * @param state   Current state of the parser
 * @param msg     Error message (if an error exists, else ignore)
 * @return  New state of the parser after parsing line
 */
ParserState parseLine(ServerConfig* config, const char* line, ParserState state, char msg[CONFIG_ERRMSG_SIZE]);

#endif /** CONFIG_CONFIG_H__ */

