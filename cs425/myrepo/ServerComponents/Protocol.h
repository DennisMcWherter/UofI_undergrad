/**
 * Protocol.h
 *
 * Protocol values
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef SERVERCOMPONENTS_PROTOCOL_H__
#define SERVERCOMPONENTS_PROTOCOL_H__

#define MAX_CHUNK_SIZE 256 // read at most 256 bytes at a time

/**
 * Server protocols
 */
struct Protocol
{
  enum GENERAL
  {
    EXIT = 0x00
  };

  enum FAILURE_DETECTION
  {
    PING  = 0x10,
    PONG  = 0x11,
    JOIN  = 0x12,
    RCONN = 0x13,
    NEWID = 0x14,
    SETMC = 0x15,
    FDCON = 0x16
  };

  enum KEYVALUE_STORE
  {
    INSERT     = 0x20,
    REMOVE     = 0x21,
    LOOKUP     = 0x22,
    REPL_INS   = 0x23,
    REPL_REM   = 0x24,
    INS_MOVIE  = 0x25,
    REPL_MOVIE = 0x26,
    LUP_MOVIE  = 0x27
  };

  enum CLIENT
  {
    REQ_FAIL     = 0x30,
    REQ_SUC      = 0x31,
    REQ_NONE     = 0x32,
    SRV_CONN     = 0x33,
    SRV_FULL     = 0x34,
    LOOKUP_NO    = 0x35,
    LOOKUP_SUC   = 0x36,
    MLOOKUP_DONE = 0x37
  };
};

#endif /** SERVERCOMPONENTS_PROTOCOL_H__ */
