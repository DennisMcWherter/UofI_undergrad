/**
 * Protocol.h
 *
 * Manage protocol information
 */

#ifndef NETWORKING_PROTOCOL_H__
#define NETWORKING_PROTOCOL_H__

typedef enum
{
  SRV_DROP_MSG  = 0x00,
  SRV_LIVE_MSG  = 0x01,
  SRV_READY_MSG = 0x02
} ServerProtocol;

typedef enum
{
  CLI_EXIT_MSG = 0x00,
  CLI_FIND_MSG = 0x01,
  CLI_INS_MSG  = 0x02,
  CLI_REM_MSG  = 0x03
} ClientProtocol;

#endif /** NETWORKING_PROTOCOL_H__ */

