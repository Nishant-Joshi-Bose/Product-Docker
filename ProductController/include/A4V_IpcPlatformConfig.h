////////////////////////////////////////////////////////////////////////////////
/// @file            A4V_IpcPlatformConfig.h
/// @brief           Type definitions and configuration for A4V IPC on Shelby.
/// @author          Dillon Johnson
/// @date            12/22/2015
/// @attention       Copyright 2015 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#define ALIGN_PACKET

#define ipc_uint8_t  uint8_t
#define ipc_uint16_t uint16_t
#define ipc_uint32_t uint32_t

#define ipc_int8_t   int8_t
#define ipc_int16_t  int16_t
#define ipc_int32_t  int32_t

#define IPC_ALIGN_PACKET __attribute__( ( aligned( 4 ) ) )

#define A4V_IPC_UART_DEVICE_NAME    "/dev/ttyO0"
#define A4V_IPC_UART_BAUD_RATE      (B921600)
#define A4V_IPC_UART_SETTINGS       (O_RDWR | O_NOCTTY | O_SYNC)