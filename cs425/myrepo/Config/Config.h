/**
 * Config.h
 *
 * Configuration file information
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef CONFIG_CONFIG_H__
#define CONFIG_CONFIG_H__

#include <map>
#include <vector>

#include "Poco/Condition.h"
#include "Poco/Mutex.h"

extern Poco::Mutex printMutex; // To synchronize printing

/**
 * Configuration data structure
 */
class Config
{
public:
  /**
   * Struct to hold server information
   */
  struct ServerInformation
  {
    std::string address;
    unsigned id;
    unsigned pubPort;
    unsigned internalPort;
    unsigned fdPort;
  };

  /**
   * Thread controlling class
   *
   * NOTE: This class is thread-safe
   */
  class ThreadControl
  {
  public:
    ThreadControl() : shutdownServer(false), reconnectedTo(false), connectedId(-1), connectedToId(-1), pubId(-1), machineCount(0) {}
    virtual ~ThreadControl() {}

    /**
     * Get the machine count
     *
     * @return  The machine count
     */
    unsigned getMachineCount();

    /**
     * Set the machine count
     *
     * @param count   The machine count
     */
    void setMachineCount(unsigned count);

    /**
     * Check if the server is still live
     *
     * NOTE: Threads should use this to determine whether or not
     *  to exit
     *
     * @return  true if still live, otherwise false.
     */
    bool isLive();

    /**
     * Get if the server was reconnected to
     *
     * @return  True if reconnected. False otherwise.
     */
    bool getReconnectedTo();

    /**
     * Set if the server was reconnected to
     *
     * NOTE: This method resets reconnectedTo to false.
     */
    void setReconnectedTo();

    /**
     * Check currently connected id
     *
     * NOTE: This method is _NOT_ thread-safe (acquire mutex first)
     *
     * @return  Currently connected server id (corresponding to server list)
     *    returns -1 if no server connected.
     */
    int getConnectedId();

    /**
     * Set currently connected id
     *
     * NOTE: This method is _NOT_ thread-safe (acquire mutex first)
     *
     * @param id    Set the id of the currently connected server. -1
     *      is equivalent to "None"
     */
    void setConnectedId(int id);

    /**
     * Get the id of the server you are connected to
     *
     * NOTE: This method is _NOT_ thread-safe (acquire mutex first)
     *
     * @return  The id of the server connected to. -1 is returned
     *    if not connected.
     */
    int getConnectedToId();

    /**
     * Set the id of the server you are connected to
     *
     * NOTE: This method is _NOT_ thread-safe (acquire mutex first)
     *
     * @param id    The id of the server connected to
     */
    void setConnectedToId(int id);

    /**
     * Get the current public ID
     *
     * @return  Current public id. Returns -1 in the
     *    event no public id exists (i.e. not connected)
     */
    int getPublicId();

    /**
     * Set the public id
     *
     * @param id    The public id to set
     */
    void setPublicId(int id);

    /**
     * Condition variable for outgoing connection
     *
     * @return  The condition variable associated with this
     *    thread control (outgoing connection)
     */
    Poco::Condition& getOutCondition();

    /**
     * Condition variable for incoming connection
     *
     * @return  The condition variable associated with
     *   thread control (incoming connection)
     */
    Poco::Condition& getInCondition();

    /**
     * Get incoming condition mutex
     *
     * @return  Incoming condition mutex
     */
    Poco::Mutex& getInMutex();

    /**
     * Get outgoing condition mutex
     *
     * @return  Outgoing condition mutex
     */
    Poco::Mutex& getOutMutex();

    /**
     * Shutdown the server
     */
    void shutdown();

  private:
    bool shutdownServer;
    bool reconnectedTo;
    int connectedId, connectedToId, pubId;
    unsigned machineCount;
    Poco::Mutex sMutex, iMutex, oMutex, pMutex, rMutex, mMutex;
    Poco::Condition inCond, outCond;
  };

  /**
   * Constructor
   */
  Config(unsigned id);

  /**
   * Destructor
   */
  virtual ~Config();

  /**
   * Get server information
   *
   * @param id    Get server information for a given server id
   * @return  A const reference to server information for the given
   *    id. If the id does not exist, the default will be used.
   */
  const ServerInformation& getServerInformation(unsigned id) const;

  /**
   * Get the next server's information
   * This is a convenience method to cycle through
   * server information. This method assumes that id's
   * are distributed from [0, N-1]. It will compute
   * the next id
   *
   * NOTE: Do _NOT_ use this method if the list only
   *    contains the current server.
   *
   * ADDITIONAL: This method is thread-safe.
   *
   * @return  The next server's information. Once the
   *    list is exhausted, this will return to the beginning
   *    of the list
   */
  const ServerInformation& getNextServer();

  /**
   * Retrieve this server's information
   *
   * @return  A const reference to the server information.
   *    If none exists, a default struct is returned.
   */
  const ServerInformation& getServerInformation() const;

  /**
   * Get internal server id for this server
   *
   * @return  The internal server id
   */
  unsigned getInternalId() const;

  /**
   * Get the next server id to use and compute a new one
   *
   * NOTE: This method is thread-safe.
   *
   * @return  The id of the next server to try
   */
  unsigned computeNextServerId();

  /**
   * Get the next server id but do not compute a new one
   *
   * @return  The id of the next server to try
   */
  unsigned getNextServerId();

   /**
    * Get the most recently computed server id
    * which was used
    *
    * @return Server id
    */
  unsigned getRecentServerId();
  
  /**
   * Add server information to the list
   *
   * NOTE: This method is _NOT_ thread-safe.
   *
   * @param info    The server information
   * @return  void
   */
  void addServer(const ServerInformation& info);

  /**
   * Set number of keys
   *
   * NOTE: This method is _NOT_ thread-safe.
   *
   * @param num   The number of total keys for the server
   */
  void setNumberOfKeys(unsigned num);

  /**
   * Get the number of keys
   *
   * @return  Number of keys in the network
   */
  unsigned getNumberOfKeys() const;

  /**
   * Get thread control object
   *
   * @return Thread control object
   */
  ThreadControl& getThreadControl();

private:
  unsigned id;
  unsigned next;
  unsigned numKeys;
  unsigned recent;
  std::vector<ServerInformation> serverInfo;
  ThreadControl threadControl;
  Poco::Mutex mutex;

  static ServerInformation defaultInfo;
};

#endif /** CONFIG_CONFIG_H__ */
