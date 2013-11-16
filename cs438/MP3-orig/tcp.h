/**
 * tcp.h
 *
 * C++ TCP Interface
 *
 * @author Dennis J. McWherter, Jr.
 */

#ifndef TCP_H__
#define TCP_H__

#include <cstring>
#include <functional>
#include <map>
#include <sys/socket.h>
#include <sys/time.h>
#include <utility>
#include <vector>
#include <queue>
#include <string>

#define PACKET_MAX_DATA (75)
#define PACKET_CONTROL    (0x01)
#define PACKET_INIT       (0x02)
#define PACKET_FIN        (0x04)
#define PACKET_RESP       (0x08)
#define PACKET_CONFIRM    (0x0f)
#define PACKET_ACK        (0x10)
#define PACKET_RETRANSMIT (0x20)

/**
 * TCP class to hold connection state, etc.
 */
class TCP
{
public:
  /**
   * Constructor
   *
   * @param host    Hostname to bind or connect to
   * @param port    Port to bind or connect to
   * @param server  Whether this socket should be used for a server or client
   *                This is false by default
   */
  TCP(const char* host, const char* port, bool server=false);

  /**
   * Destructor
   */
  virtual ~TCP();

  /**
   * Connect to the erver specified in the constructor
   * NOTE: This method is only valid for clients
   *
   * @return  The socket file descriptor on success -1 otherwise.
   */
  virtual int connect();

  /**
   * Start listening for clients
   * NOTE: This method is only valid for servers
   *
   * @return  The socket file descriptor on success -1 otherwise.
   */
  virtual int listen();

  /**
   * Close the open connection (if any)
   */
  virtual void closeConnection();

  /**
   * Accept a client connection
   * NOTE: Method only valid for servers
   *
   * @return  File descriptor of newly connected client.
   *          -1 on error.
   */
  virtual int accept();

  /**
   * Close connection with specified client
   *
   * @param sock    The file descriptor to the client
   */
  virtual void closeClientConnection(int sock);

  /**
   * Get the current main file descriptor
   *
   * @return  File descriptor of active connecction. -1
   *          if no active connection.
   */
  virtual int getFileDescriptor() const { return sockfd; }

  /**
   * Send from a socket - this handles responses, etc.
   *
   * @param sock    Socket to send data from
   * @param data    Data to send to client
   * @param len     Data length
   *
   * @return  The number of bytes sent or -1 on error
   */
  virtual int send(int sock, void* data, size_t len);

  /**
   * Receive data from a socket
   *
   * @param sock    Socket to read from
   * @param data    Input buffer to fille
   * @param len     Length of the input buffer
   *
   * @return  The number of bytes received. -1 on error
   */
  virtual int recv(int sock, void* data, size_t len);

  /**
   * Send an end control message to client
   *
   * @param sock    Socket to send fin to
   */
  virtual void closeClient(int sock);

private:
  /** Private structures */
  /**
   * Client state information
   */
  struct ClientState
  {
    int seqno;
    int totalsent;
    struct sockaddr addr;
  };

  /**
   * TCP Control Packet
   *
   * msg format (bit-vector):
   *  ** By bit position:
   *     0 - CONTROL bit (whether this is a control packet or not)
   *     1 - INIT bit (start a connetion)
   *     2 - FIN bit (close a connection)
   *     3 - INIT_RESP (respond with new information)
   *     4 - CONFIRM bit (client has received new port to listen on)
   *     5 - ACK bit 
   *     6 - RETRANSMIT bit (diagnostic only - if packet was retransmitted)
   *     7 - Unassigned
   */
  struct Packet
  {
    Packet()
      : metadata(0), start(0), size(0)
    {
      memset(data, 0, sizeof(data));
    }
    Packet(const void* in, size_t len, unsigned st)
      : metadata(0)
    {
      start = st;
      size  = (len < sizeof(data)) ? len : sizeof(data);
      memset(data, 0, PACKET_MAX_DATA);
      memcpy(data, in, size);
    }
    Packet(const Packet& rhs)
    {
      metadata = rhs.metadata;
      start    = rhs.start;
      size     = rhs.size;
      memset(data, 0, PACKET_MAX_DATA);
      memcpy(data, rhs.data, rhs.size);
    }
    Packet& operator=(const Packet& rhs)
    {
      if(&rhs != this) {
        metadata = rhs.metadata;
        start    = rhs.start;
        size     = rhs.size;
        memset(data, 0, PACKET_MAX_DATA);
        memcpy(data, rhs.data, rhs.size);
      }
      return *this;
    }
    virtual ~Packet() {}

    bool operator<(const Packet& rhs) const
    {
      return this->start > rhs.start;
    }

    // Packet data
    unsigned char metadata;

    // If not a control packet, then these fields are used
    int start;
//    unsigned segend;
    unsigned size;
    unsigned char data[PACKET_MAX_DATA];
  };

  static void printFileBuffer(std::queue<Packet>& buffer);
  static void printSendBuffer(std::priority_queue<Packet>& buffer);

  /** Private methods */
  /**
   * Initialize a socket
   *
   * NOTE: This method only works on sockfd's which are uninitialized (i.e. == -1)
   *
   * @return  The socket file descriptor on success or -1 otherwise.
   */
  int initSocket();

  /**
   * Get state of a client
   *
   * @param sock    Socket to get state for
   *
   * @return  A pointer to the state of the client state
   */
  ClientState* getState(int sock);

  /**
   * Send a packet
   *
   * @param sock    Socket to send the data on
   * @param state   The client to send the packet to
   * @param packet  The packet to send
   *
   * @return  Number of bytes sent, or -1 on error.
   */
  int sendPacket(int sock, ClientState* state, Packet& packet);

  /**
   * Receive a packet
   *
   * @param sock    Socket to receive data on
   * @param state   State of the connection
   * @param packet  Packet structure to fill
   * @param theirs  Senders address
   * @param tlen    Senders address length
   *
   * @return  Number of bytes received. -1 on error.
   */
  int recvPacket(int sock, ClientState* state, Packet& packet, struct sockaddr& theirs, socklen_t& tlen);

  /**
   * Send all the possible packets
   *
   * @param sock
   * @param state
   * @param fileBuffer
   */
  void sendAllPossiblePackets(int sock, ClientState* state, std::queue<Packet>& fileBuffer);

  void sendAllSendBufferPackets(int sock, ClientState* state, std::priority_queue<Packet>& buffer);

  /**
   * Retransmit packet
   *
   * @param sock
   * @param state
   */
  void retransmitPacket(int sock, ClientState* state);

  /**
   * Flatten a TCP packet to a buffer
   * (buffer must be at least sizeof(Packet))
   *
   * @param packet    Packet to translate
   * @param buffer    Buffer to copy the packet into
   */
  void flattenPacket(const Packet& packet, void* buffer) const;

  /**
   * Expand a buffer back to packet form
   *
   * @param buffer    Buffer to expand
   * @param len       Size of the buffer
   *
   * @return  The data in packet form
   */
  Packet expandPacket(const void* buffer, size_t len) const;

  /**
   * Get next (recv) queued packet if one available
   *
   * @param sock    Socket to check for queued packets
   * @param packet  Packet to fill if possible
   *
   * @return  True if a packet was available, false otherwis
   */
  bool getNextRQueue(int sock, Packet& packet);

  /**
   * Clear the queues
   *
   * @param sock    Corresponding socket to clear for
   */
  void clearQueues(int sock);

  /**
   * Process the ACK
   *
   * @param ack   Ack packet
   */
  int processAck(Packet& ack);

  /**
   * Consume send buffer and soul if you let it.
   *
   * @param ack     Ack value
   * @param sock    Socket
   * @param state   Connection state
   *
   * @return  The total size in bytes of un ACK'ed data after
   *          the buffer is updated.
   */
  int consumeSendBuffer(int ack, int sock, ClientState* state);

  /**
   * Sum the send buffer
   */
  int sumSendBuffer(int sock);

  /**
   * Handle packet timeouts
   */
  void timeout();

  unsigned cumulativeAck();

  std::string getRelativeTime() const;

  /**
   * Simple reverse comparator
   */
  class ReversePacketCompare : public std::binary_function<Packet, Packet, bool>
  {
  public:
    bool operator()(const Packet& lhs, const Packet& rhs) const
    {
      return lhs.start < rhs.start;
    }
  };

  typedef std::priority_queue<TCP::Packet, std::vector<TCP::Packet>, TCP::ReversePacketCompare> p_pq;

  /** Private vars */
  int sockfd;
  const char* host;
  const char* port;
  const char* phase;
  struct timeval baseTime;
  bool isServer;
  unsigned short cwnd;
  unsigned short ssthresh;
  int canSend;
  float RTT;
  int RTO;
  int lastAck;
  int lastSent;
  int dupackCount;
  bool retransmitting;
  bool inprocofrtrans;
  bool closing;
  std::map<int, ClientState> clients;
  std::map<int, std::priority_queue<Packet> > sendBuffer;
  std::map<int, p_pq > recvBuffer;

  static TCP* instance; // NOTE: This is dangerous. But the MP only requires one instance per program
                        // But remember that this would not work for many instances
};


#endif /** TCP_H__ */

