/**
 * tcp.cpp
 *
 * TCP Implementation
 *
 * @author Dennis J. McWherter, Jr.
 */

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream> // for debugging
#include <netdb.h>
#include <netinet/in.h>
#include <queue>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include "mp3.hh"
#include "tcp.h"

#define MAX_WND (65535)
#define MSS (PACKET_MAX_DATA)
#define TIMEOUT (3)

using namespace std;


void TCP::printFileBuffer(queue<Packet>& buffer)
{
  unsigned elems = buffer.size();
  for(unsigned i = 0 ; i < elems ; ++i) {
    Packet p(buffer.front());
    buffer.pop();
    buffer.push(p);
    cout << "PRINTING File Buffer" << endl;
    cout<< "Packet size: "<< p.size << " ; Packet start: "<< p.start <<
    " ; Packet data: "<< p.data << endl << endl;
  }
}

void TCP::printSendBuffer(priority_queue<Packet>& buffer)
{
  queue<Packet> tmp;

  while(!buffer.empty()) {
    tmp.push(buffer.top());
    buffer.pop();
  }

  while(!tmp.empty()) {
    Packet p(tmp.front());
    tmp.pop();
    buffer.push(p);
    cout << "PRINTING Send Buffer" << endl;
    cout<< "Packet size: "<< p.size << " ; Packet start: "<< p.start <<
    " ; Packet data: "<< p.data << endl << endl;
  }
}

TCP* TCP::instance = NULL;

TCP::TCP(const char* host, const char* port, bool server)
  : sockfd(-1), host(host), port(port), isServer(server),
  cwnd(MSS), ssthresh(MAX_WND), canSend(cwnd), RTT(1.f), RTO(2),
  lastAck(0), dupackCount(0), retransmitting(false), inprocofrtrans(false),
  closing(false)
{
  TCP::instance = this;
  mp3_init();
//  signal(SIGINT, TCP::timeout_handler); // Use the timeout callback to handle signals
}

TCP::~TCP()
{
  if(TCP::instance == this)
    TCP::instance = NULL; // Probably a useless case since we should only have
                          // one instance at a time. But really, probably only
                          // one instance per execution
  closeConnection(); // Make sure we clean up
}

int TCP::connect()
{
  TCP::Packet req, resp;
  struct sockaddr theirs;
  socklen_t tlen = sizeof(theirs);
  int attempts = 3;
  struct timeval tv;
  fd_set fdset;

  if(isServer || sockfd != -1 || clients.size() == 1)
    return -1;

  assert(clients.size() < 1);

  if(initSocket() < 0)
    return -1;

  req.metadata = PACKET_CONTROL | PACKET_INIT;
  req.size = 1;

  // Keep attempting to get data
  while(attempts > 0) {
    if(sendPacket(sockfd, &clients[sockfd], req) < 0)
      return -1;

    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    tv.tv_sec  = 1;
    tv.tv_usec = 0;
    select(sockfd + 1, &fdset, NULL, NULL, &tv);

    if(FD_ISSET(sockfd, &fdset)) {
      if(recvPacket(sockfd, &clients[sockfd], resp, theirs, tlen) < 0)
        return -1;
      // Buffer packet
      recvBuffer[sockfd].push(resp);
      break;
    }

    attempts--;
  }

  clients[sockfd].seqno = 0;
  clients[sockfd].totalsent = 0;
  clients[sockfd].addr.sa_family = theirs.sa_family;
  memcpy(&clients[sockfd].addr.sa_data, theirs.sa_data, sizeof(theirs.sa_data));
  return (attempts) ? sockfd : -1;
}

int TCP::listen()
{
  if(!isServer || sockfd != -1)
    return -1;

  if(initSocket() < 0) // Initialize our socket
    return -1;

  // The idea here is to use a UDP socket only for listening
  // After it is bound, simply return the sockfd

  return sockfd;
}

void TCP::closeConnection()
{
  if(sockfd != -1) {
    if(isServer) {
      map<int, ClientState>::iterator it;
      for(it = clients.begin() ; it != clients.end() ; ++it) {
        int client = it->first;
        // TODO: Send each client packet saying we're done
        close(client);
      }
      clients.empty(); // Clear list of clients
    } else {
      // TODO: Send connection terminated packet (i.e. FIN?)
    }
    close(sockfd);
    sockfd = -1;
  }
}

int TCP::accept()
{
  TCP::Packet packet, resp;
  struct sockaddr their_addr;
  socklen_t their_addrlen = sizeof(their_addr);

  if(sockfd < 0 || !isServer)
    return -1;

  memset(&their_addr, 0, their_addrlen);

  clients[sockfd].seqno = 0;
  clients[sockfd].totalsent = 0;

  // NOTE: This is a TCP-like connection: since 3-way handshake
  // was not part of the spec, it is not fully implemented here
  // TODO: Wrap the receive call
  if(recvPacket(sockfd, &clients[sockfd], packet, their_addr, their_addrlen) < 0) {
    return -1; 
  }

  if(!(packet.metadata & PACKET_INIT))
    return -1; // This is not a connection request

  // Hold state for this client
  // Start all seqno's = 0 for now
  ClientState state;
  state.seqno = 0;
  state.totalsent = 0;
  state.addr.sa_family = their_addr.sa_family;
  memcpy(&state.addr.sa_data, their_addr.sa_data, sizeof(state.addr.sa_data));
  clients[sockfd] = state;

  return sockfd;//newsock;
}

void TCP::closeClientConnection(int sock)
{
  if(!isServer)
    return; // Only servers can do this

  map<int, ClientState>::iterator it(clients.find(sock));

  if(it == clients.end())
    return; // If active session exists, we are not responsible

  // Not super concerned whether this fails or not, just try to be nice
  // TODO: Maybe this should be a data packet?
  Packet fin;
  fin.metadata = PACKET_FIN;
  // TODO: Send?

  close(sock);
  clients.erase(it);
}

int TCP::send(int sock, void* data, size_t len)
{
  ClientState* state = getState(sock);
  int ret = 0;
  fd_set read_fds;
  struct timeval tv;
  struct sockaddr theirs;
  socklen_t tlen = sizeof(theirs);
  queue<Packet> fileBuffer;
  unsigned totalSent = 0;
  bool keepSending = true;

  if(state == NULL)
    return -1; // We don't have state for this socket

  // Queue of packets
  while(totalSent < len) {
    size_t amt = ((len - totalSent) > PACKET_MAX_DATA) ? PACKET_MAX_DATA : (len - totalSent);
    fileBuffer.push(Packet(((char*)data) + totalSent, amt, totalSent));
    totalSent += amt;
  }

  sendAllPossiblePackets(sockfd, state, fileBuffer);

  while(keepSending) {
    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);
    tv.tv_sec  = 0;
    tv.tv_usec = 200000; // TODO - update RTO

    if(select(sock + 1, &read_fds, NULL, NULL, &tv) != -1) {
      // Good logic
      if(FD_ISSET(sock, &read_fds)) { // Ack?
        Packet ack;
        recvPacket(sockfd, state, ack, theirs, tlen);
        // TODO: Maybe check if this is an ack or not

        // Send what we need to
        keepSending = ack.start < len;
        if(keepSending) {
          int res = processAck(ack);
          if(res == -1 && dupackCount == 3)
            retransmitPacket(sockfd, state);
          if(res != -2)
            sendAllPossiblePackets(sockfd, state, fileBuffer);
        }
      } else { // Timeout
        timeout();
        //if(sendBuffer[sockfd].empty())
          //sendAllPossiblePackets(sockfd, state, fileBuffer);
        //else
          sendAllSendBufferPackets(sockfd, state, sendBuffer[sockfd]);
      }
    } else {
      break; // Die
    }
  }

  return ret;
}

int TCP::processAck(Packet& ack)
{
  cout<< "--PROCESSING ACK: " << "ack.start = " << ack.start << " : lastAck = " << lastAck << endl << endl;
  if(ack.start > lastAck) { // Not a dupack
    bool slowstart = cwnd < ssthresh;

    dupackCount = 0;
    lastAck     = ack.start;

    // Actually update send buffer
    int cumqsize = consumeSendBuffer(lastAck, sockfd, getState(sockfd));

    if(retransmitting) { // Fast retransmit
      cwnd = ssthresh;
      retransmitting = false;
    } else if(slowstart) { // Slow start
      cwnd += MSS;
    } else { // Congestion Avoidance
      cwnd += ((MSS * MSS) / cwnd);
    }

    canSend = cwnd - cumqsize;
    canSend = (canSend < 0) ? 0 : canSend;

    return canSend;
  } else if(lastAck == ack.start) { // Dupack
    dupackCount++;
    return -1;
  }

  return -2;
}

void TCP::retransmitPacket(int sock, ClientState* state)
{
  if(state == NULL || sendBuffer[sock].empty())
    return;
  
  Packet p(sendBuffer[sock].top());
  int cumqsize = sumSendBuffer(sock);
  retransmitting = true;      
  ssthresh = ((cwnd / 2) > (2 * MSS)) ? (cwnd / 2) : (2 * MSS);
  cwnd = ssthresh + (3 * MSS);
  canSend = cwnd - cumqsize;
  canSend -= p.size;
  canSend = (canSend < 0) ? 0 : canSend;

//  sendBuffer[sock].pop();
  sendPacket(sock, state, p);
}

void TCP::sendAllPossiblePackets(int sock, ClientState* state, queue<Packet>& fileBuffer)
{
  cout << "-<--SEND ALL POSSIBLE->--" << endl;
  assert(state);

  while(!fileBuffer.empty() && fileBuffer.front().size <= canSend) {
    Packet p(fileBuffer.front());
    fileBuffer.pop();
    canSend -= p.size;
    sendPacket(sock, state, p);
    cout<< "!- SENDALL: This packet start : " << fileBuffer.front().start << endl << endl;
  }
}

void TCP::sendAllSendBufferPackets(int sock, ClientState* state, priority_queue<Packet>& buffer)
{
  if(state == NULL)
    return;

  cout<< " -<--SEND ALL SEND BUFFER POSSIBLE->--" << endl;
  while(!buffer.empty() && canSend >= buffer.top().size) {
    Packet p(buffer.top());
    canSend -= p.size;
    // Should we pop?
    buffer.pop();
    cout<< "Sending something" << endl;
    sendPacket(sock, state, p);
  }
}

int TCP::sumSendBuffer(int sock)
{
  priority_queue<Packet>& q = sendBuffer[sock];
  queue<Packet> packets;
  int ret = 0;

  while(!q.empty()) {
    packets.push(q.top());
    q.pop();
  }

  while(!packets.empty()) {
    Packet p(packets.front());
    packets.pop();
    q.push(p);
    ret += p.size;
  }

  return ret;
}

void TCP::timeout()
{
  cout<< "TODO: Handle signal (back to slow start)!!!" << endl << endl;
  // Following some stuff from RFC6298
  // Exponential backoff on the RTO since we timed out
  canSend  = MSS;
  ssthresh = ((cwnd / 2) < (2 * MSS)) ? 2 * MSS : (cwnd / 2);
//  RTO  = (RTO * 2 > 60) ? 60 : RTO * 2;
  cwnd = MSS;
}

int TCP::consumeSendBuffer(int val, int sock, ClientState* state)
{
  cout << "--CONSUME SEND BUFFER--" << endl;
  priority_queue<Packet>& qu = sendBuffer[sock];
  int cum = 0;
  queue<Packet> q;

  if(state == NULL)
    return -1;

  if(qu.empty())
    return 0;

  cout<< "---SendBuf: Start: "<< qu.top().start << endl;
  while(!qu.empty()) {
    q.push(qu.top());
    qu.pop();
  }
  cout<< "---SendBuf 2: Start: "<< q.front().start << endl
  << "---LASTACK: " << val << endl;

  while(!q.empty()) {
    int segend = q.front().start + q.front().size;
    if(segend < val) {
      cout<< "----Removing: "<< q.front().start << endl;
      q.pop();
    } else {
      cum += q.front().size;
      cout<< "----Onto el coo: "<< q.front().start << endl;
      qu.push(q.front());
      q.pop();
    }
  }

  if(!qu.empty())
    cout<< "---Final SendBuf Start: "<< qu.top().start << endl << endl;

  return cum;
}

int TCP::sendPacket(int sock, ClientState* state, Packet& packet)
{
  int ret = -1;
  Packet resp;
  unsigned char buffer[sizeof(packet)];
  memset(buffer, 0, sizeof(buffer));

  // Buffer the packet until it has been ack'd
  //   but don't buffer control packets
  if(!(packet.metadata & PACKET_CONTROL)) { // Control packets "starts" will be defined manually for their purposes
    if(packet.start == -1) // Only if start is not already set
      packet.start = state->totalsent;
  }

  flattenPacket(packet, buffer);

  ret = mp3_sendto(sock, buffer, sizeof(buffer), 0, &state->addr, sizeof(state->addr));

  if(ret >= 0 && !(packet.metadata & PACKET_CONTROL)) {
    if(!(packet.metadata & PACKET_RETRANSMIT))
      state->totalsent += packet.size;

    // Queue this but set that it's retransmitted if sent again
    packet.metadata |= PACKET_RETRANSMIT;
    sendBuffer[sock].push(packet);
    cout<< "!!!Push packet to send buf: start: " << packet.start << " ; size " << packet.size << "Last Ack: "<< lastAck << " ; " << "cwnd: "<< cwnd << endl << " DATA: " << packet.data << endl <<endl;
  } else {
    cout<< "!!!Control Packet packet: start: " << packet.start << hex << " ; metadata: 0x"<< (int)packet.metadata << dec
    << " ; sendto val: " << ret << endl << endl;
  }

  return ret;
}

int TCP::recv(int sock, void* data, size_t len)
{
  ClientState* state = getState(sock);
  struct sockaddr theirs;
  socklen_t tlen = sizeof(theirs);
  static int lastSent = 0;

  if(state == NULL)
    return -1;

  Packet packet;
  int ret = 1;
  while((ret = recvPacket(sock, state, packet, theirs, tlen)) > 0) {
    if(!(packet.metadata & PACKET_CONTROL) &&
      state->seqno == (packet.start + packet.size)
      && state->seqno != lastSent) { // Make sure this is not a control packet
      // ASSUMPTION:  packet data <= buffer (i.e. buffer >= PACKET_MAX_DATA)
      lastSent = state->seqno;
      memset(data, 0, len);
      memcpy(data, packet.data, packet.size);
      cout<< "Sent up recent data (from ----) :::::::: Packet.start " << packet.start << " ; packet.size" << packet.size << endl;
      break;
    }
  }

  return ret;
}

void TCP::closeClient(int sock)
{
  int attempts = 20; // Try to send it 3 times and hope they get it
  Packet p;
  p.metadata = PACKET_CONTROL | PACKET_FIN;
  ClientState* state = getState(sock);

  closing = true;

  if(state == NULL)
    return;

  cout<< "Closing client" << endl;

  while(attempts-- > 0) {
    sendPacket(sock, state, p);
  }

  cout<< "Done except for clear queues..." << endl;

  clearQueues(sock);
}

/** Private methods */
int TCP::initSocket()
{
  struct addrinfo hints;
  struct addrinfo* info = NULL;

  if(sockfd != -1)
    return sockfd; // We have an active connection

  memset(&hints, 0, sizeof(hints));

  hints.ai_family   = AF_INET; // IPv4 only
  hints.ai_socktype = SOCK_DGRAM; // UDP
  hints.ai_protocol = IPPROTO_UDP;
  if(isServer && host == NULL)
    hints.ai_flags  = AI_PASSIVE; // Fill in IP automatically if we don't provide one
  if(getaddrinfo(host, port, &hints, &info) < 0)
    return -1;

  sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

  if(isServer) { // Bind
    if(bind(sockfd, info->ai_addr, info->ai_addrlen) < 0)
      return -1;
  } else { // Bind on next available port (so server can directly communicate)
    struct addrinfo* res = NULL;
    if(getaddrinfo("localhost", 0, &hints, &res) < 0)
      return -1;
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      freeaddrinfo(info);
      freeaddrinfo(res);
      return -1;
    }
    freeaddrinfo(res);

    // Our client is only the server - add this
    ClientState state;
    state.seqno = 0;
    state.addr.sa_family = info->ai_addr->sa_family;
    memcpy(&state.addr.sa_data, info->ai_addr->sa_data, sizeof(state.addr.sa_data));
    clients[sockfd] = state;
  }

  freeaddrinfo(info);

  return sockfd;
}

TCP::ClientState* TCP::getState(int sock)
{
  map<int, ClientState>::iterator it(clients.find(sockfd));

  if(it == clients.end())
    return NULL;

  return &it->second;
}

int TCP::recvPacket(int sock, ClientState* state, Packet& packet, struct sockaddr& theirs, socklen_t& tlen)
{
  int ret = -5; // Arbitrary "unset" value
  unsigned char buffer[sizeof(packet)];

  // Make sure you try to flush queue first
  if(getNextRQueue(sock, packet)) {
    ret = packet.size;
    state->seqno += ret;
  }
  
  // Wait until we have something useful
  while(ret == -5) {
    cout<< "..Receiving.." <<endl;
    ret = recvfrom(sock, buffer, sizeof(buffer), 0, &theirs, &tlen);
    cout<< "....Received ("<< ret << ")..."<<endl;

    if(ret == sizeof(packet)) { // Received a whole packet
      packet = expandPacket(buffer, ret);
      if(!(packet.metadata & PACKET_CONTROL)) {
        // Queue if not the next packet to be served up
        if(packet.start > state->seqno) {
          recvBuffer[sock].push(packet);
          cout<< "....Queuing... Recv'd Packet: " << packet.start << " vs. Seq: " << state->seqno 
          << "DATA: " << packet.data << endl;
        } else if(packet.start == state->seqno) {
          ret = packet.size;
          cout<< state->seqno << " + "<< ret << " = " << (state->seqno + ret) << endl;
          state->seqno += packet.size;
          //cout<< "true? "<< state->seqno << endl;
          // TODO: Make sure this is actually the right number - we may
          // have received more packets to use?
        } else {
          cout<< "(((DUP PACKET)))" << endl << packet.data << endl << "----"<<endl;
        }

        // Send an ACK back to the server
        Packet ack;
        ack.metadata = PACKET_CONTROL | PACKET_ACK;
        ack.start = state->seqno + 1; // Field shows what we are ack'ing
        cout<< "....Sending ack start: "<< ack.start << endl;
        ack.size  = 1;
        sendPacket(sock, state, ack);
      } else { // Process
        ret = packet.size;
        if(packet.metadata & PACKET_FIN) {
          ret = 0;
        } else if(packet.metadata & PACKET_ACK) {
          // TODO: Confirm ACK
          cout<< "....Received ACK: " << packet.start << ":" << packet.size << endl << endl;
          processAck(packet);
          //updateBufferAck(packet.start, sock, state);
//          processAck(packet);
        }
      }
    }
  }

  return ret;
}

void TCP::flattenPacket(const Packet& packet, void* buf) const
{
  char* buffer = static_cast<char*>(buf);
  unsigned usage = 0;
  if(buffer == NULL)
    return;
  buffer[usage] = packet.metadata;
  usage += sizeof(char);
  *((int*)&buffer[usage]) = packet.start;
  usage += sizeof(int); 
  *((unsigned*)&buffer[usage]) = packet.size;
  usage += sizeof(unsigned);
  memcpy(&buffer[usage], packet.data, PACKET_MAX_DATA);
}

TCP::Packet TCP::expandPacket(const void* buf, size_t len) const
{
  const char* buffer = static_cast<const char*>(buf);
  unsigned usage = 0;
  Packet packet;
  if(buf == NULL || len != sizeof(Packet))
    return packet;
  packet.metadata = buffer[usage];
  usage += sizeof(char);
  packet.start = *((int*)&buffer[usage]);
  usage += sizeof(int);
  packet.size = *((unsigned*)&buffer[usage]);
  usage += sizeof(unsigned);
  memcpy(packet.data, &buffer[usage], packet.size);
  return packet;
}

bool TCP::getNextRQueue(int sock, Packet& packet)
{
  p_pq& queue = recvBuffer[sock];
  ClientState* state = getState(sock);

  if(queue.empty() || state == NULL)
    return false;

  while(!queue.empty() && queue.top().start < state->seqno)
    queue.pop(); // Drop these - duplicates?

  cout<< "Looking" << endl;
  if(!queue.empty() && queue.top().start == state->seqno) {
    packet = Packet(queue.top());
    queue.pop();
    while(!queue.empty() && queue.top().start == packet.start)
      queue.pop(); // Did we queue duplicates??
    return true;
  }

  return false;
}

void TCP::clearQueues(int sock)
{
  p_pq emptyp;
  priority_queue<Packet> emptyq;
  swap(recvBuffer[sock], emptyp);
  swap(sendBuffer[sock], emptyq);
}

