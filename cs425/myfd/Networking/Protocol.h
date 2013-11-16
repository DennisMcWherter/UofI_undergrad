/**
 * Protocol.h
 *
 * Manage protocol information
 */

#ifndef NETWORKING_PROTOCOL_H__
#define NETWORKING_PROTOCOL_H__

typedef enum
{
  SRV_DROP_MSG = 0x0,
  SRV_LIVE_MSG = 0x01
} ServerProtocol;

typedef enum
{
  CLI_MSG
} ClientProtocol;

#endif /** NETWORKING_PROTOCOL_H__ */

