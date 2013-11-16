/**
 * KeyValueStore.h
 *
 * Key value store component
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef SERVERCOMPONENTS_KEYVALUESTORE_H__
#define SERVERCOMPONENTS_KEYVALUESTORE_H__

#include <string>
#include <utility>

#include "Protocol.h"
#include "../Config/Config.h"

#include "Poco/Mutex.h"
#include "Poco/Runnable.h"
#include "Poco/Task.h"
#include "Poco/Net/StreamSocket.h"

#define MAX_REPLICATE_MACHINES 2 // Up to 2 additional machines should get replicated data

class KeyValueStore : public Poco::Runnable
{
public:
  /**
   * Constructor
   *
   * @param config    Configuration information
   */
  KeyValueStore(Config& config);

  /**
   * Destructor
   */
  virtual ~KeyValueStore();

  /**
   * Thread entry point
   */
  virtual void run();

private:
  /**
   * String to int conversion
   *
   * @param str   String to conver
   * @return integer value
   */
  static int stringToInt(const std::string& str);

  /**
   * Integer to string conversion
   *
   * @param val   Integer value
   * @return  String value
   */
  static std::string intToString(int val);

  /**
   * Simple extract text between quotes
   *
   * @param str   Quoted string
   * @return  Text between quotes
   */
  static std::string extractQuotes(const std::string& str);

  /**
   * Tokenize a string
   * 
   * @param str   The string to tokenize
   * @param delim The delimeter to break tokens by
   * @return  A vector of the tokens
   */
  static std::vector<std::string> tokenize(const std::string& str, char delim=' ');

  /**
   * Insert element into movie database
   *
   * @param client    Open client socket
   * @param buf       The string to insert
   * @param control   Thread control object
   * @param forward   Whether the request should ever be forwarded or not
   */
  static void insertMovieDB(Poco::Net::StreamSocket& client, const std::string& buf, Config::ThreadControl& control, bool forward=true);

  /**
   * Handle a request
   *
   * @param client    Open client socket
   * @param control   Thread control
   * @param buf       A valid pointer (should be initialized to NULL).
   *          NOTE: memory will be allocated/freed for this point
   *          unless exception is thrown (then needs to be freed)
   *
   * @return  True if the request was to exit. False otherwise.
   */
  static bool handleRequest(Poco::Net::StreamSocket& client, Config::ThreadControl& control, char*& buf);

  /** MOVIE Commands */
  /**
   * Handle an insert request
   *
   * @param client    Open client connection
   * @param buf       The input request buffer
   * @param control   Thread control object
   */
  static void handleInsertMovie(Poco::Net::StreamSocket& client, const std::string& buf, Config::ThreadControl& control);
  
  /**
   * Handle a lookup movie request
   *
   * @param client    Open client connection
   * @param buf       The input request buffer
   * @param control   Control thread object
   */
  static void handleLookupMovie(Poco::Net::StreamSocket& client, const std::string& buf, Config::ThreadControl& control);

  /**
   * Process the query
   *
   * @param client    Open client socket
   * @param control   Thread control
   * @param req       Request code
   * @param buf       The whole input line from the user
   */
  static void handleQuery(Poco::Net::StreamSocket& client, Config::ThreadControl& control, char req, const std::string& buf);

  /**
   * Handle an insert request
   *
   * @param client    Open client connection
   * @param buf       The input request buffer
   * @param control   Thread control object
   */
  static void handleInsert(Poco::Net::StreamSocket& client, const std::string& buf, Config::ThreadControl& control);

  /**
   * Handle a removal request
   *
   * @param client    Open client connection
   * @param buf       The input request buffer
   * @param control   Thread control object
   */
  static void handleRemove(Poco::Net::StreamSocket& client, const std::string& buf, Config::ThreadControl& control);

  /**
   * Handle a lookup request
   *
   * @param client    Open client connection
   * @param buf       The input request buffer
   * @param control   Control thread object
   */
  static void handleLookup(Poco::Net::StreamSocket& client, const std::string& buf, Config::ThreadControl& control);
  
  /**
   * Handle a movie insert replication request
   *
   * @param rem       The original request (without count attached)
   * @param count     The current count
   * @param control   Thread control object
   */
  static void handleMovieInsRepl(const std::string& rem, int count, Config::ThreadControl& control);

  /**
   * Handle an insert replication request
   *
   * @param rem       The original request (without count attached)
   * @param count     The current count
   * @param control   Thread control object
   */
  static void handleInsRepl(const std::string& rem, int count, Config::ThreadControl& control);

  /**
   * Handle a remove replication request
   *
   * @param rem       The original request (without count attached)
   * @param count     The current count
   * @param control   Thread control object
   */
  static void handleRemRepl(const std::string& rem, int count, Config::ThreadControl& control);

  /**
   * Create the replicate message
   *
   * @param req         The request to replicate
   * @param remaining   Number of remaining machines to replicate to
   */
  static std::string createReplMsg(const std::string& req, int remaining=MAX_REPLICATE_MACHINES);

  /**
   * Check if a given server is responsible for a key
   *
   * @param key       The key to check
   * @param control   Thread control object
   * @return  True if responsible, false otherwise.
   */
  static bool isResponsible(const std::string& key, Config::ThreadControl& control);

  /**
   * Do replication on a single element
   *
   * @param type      The protocol code type
   * @param req       The request line
   * @param control   Thread control
   * @param remaining The number of machines remaining to replicate to
   */
  static void doReplicate(Protocol::KEYVALUE_STORE type, const std::string& req, Config::ThreadControl& control, int remaining=MAX_REPLICATE_MACHINES);

  /**
   * Replicate the whole database with a new connection
   *
   * @param sock      Open socket
   * @param mCount    Machine count
   *        Default is an arbitrary number > 2
   */
  static void replicateDB(Poco::Net::StreamSocket& sock, unsigned mCount=4);

  /**
   * Forward request and wait for response
   *
   * @param type      The protocol code type
   * @param req       The request line
   * @param control   Thread control
   * @return  The value from the forwarded request
   */
  static std::pair<char, std::string> forwardRequest(Protocol::KEYVALUE_STORE type, const std::string& req, Config::ThreadControl& control);

  /**
   * Forward a long request and wait for a long response
   *
   * @param type      The protocol code type
   * @param req       The request line
   * @param control   Thread control
   * @return  The value from the forwarded request
   */
  static std::vector<std::pair<char, std::string> > forwardLongRequest(Protocol::KEYVALUE_STORE type, const std::string& req, Config::ThreadControl& control);
  
  /**
   * Send request
   *
   * @param sock      Open client socket
   * @param req       The grouped request
   */
  static inline void sendRequest(Poco::Net::StreamSocket& sock, const std::pair<char, std::string>& req);

  /**
   * Receive response
   *
   * @param sock      Open socket
   * @param buf       A valid pointer (should be initialized to NULL).
   *          NOTE: memory will be allocated/freed for this point
   *          unless exception is thrown (then needs to be freed)
   * @return  A pair of return values from the server
   */
  static inline std::pair<char, std::string> rcvResponse(Poco::Net::StreamSocket& sock, char*& buf);

  /**
   * Internal thread 
   * Handles server forward requests
   */
  class InternalThread : public Poco::Runnable
  {
  public:
    InternalThread(Config& conf, bool incoming) : config(conf), incoming(incoming) {}
    virtual ~InternalThread() {}
    virtual void run();
    virtual void incomingRoutine();
    virtual void outgoingRoutine();
    virtual inline void replyForward();

    /**
     * What to do with incoming connection
     */
    class Incoming : public Poco::Task
    {
    public:
      Incoming() : Task("client"), control(NULL) {} // Do NOT use this constructor other than for faulty init
      Incoming(const Poco::Net::StreamSocket& sock, Config::ThreadControl* ctrl) : Task("client"), client(sock), control(ctrl) {}
      virtual void runTask();
      Incoming& operator=(const Incoming& rhs)
      {
        if(&rhs != this) {
          client  = rhs.client;
          control = rhs.control;
        }
        return *this;
      }

    private:
      Poco::Net::StreamSocket client;
      Config::ThreadControl* control;
    };

  private:
    Config& config;
    bool incoming;
    Poco::Net::StreamSocket sock;
  };

  /**
   * Public thread
   * Handles clients
   */
  class PublicThread : public Poco::Runnable
  {
  public:
    PublicThread(Config& conf) : config(conf) {}
    virtual ~PublicThread() {}
    virtual void run();
    
  private:
    /**
     * Handle client thread
     */
    class HandleClient : public Poco::Runnable
    {
    public:
      HandleClient() : control(NULL) {} // Do not actually use this constructor!
      HandleClient(const Poco::Net::StreamSocket& sock, Config::ThreadControl& ctrl) : client(sock), control(&ctrl) {}
      virtual ~HandleClient() {}
      virtual void run();

    private:
      Poco::Net::StreamSocket client;
      Config::ThreadControl* control;
    };

    Config& config;
  };

  Config& config;
};

#endif /** SERVERCOMPONENTS_KEYVALUESTORE_H__ */
