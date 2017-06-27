/*
 * @file
 *
 * @brief
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#ifndef COMMS_H_
#define COMMS_H_

#include <project.h>
#include "util.h"

void CommsInit(void);
void CommsSendData(const uint8_t *buffer);
void CommsHandler(void);
void CommsSendStatus(BOOL status);

typedef enum {
    COMMS_COMMAND_GETVERSION = 0,
//    COMMS_COMMAND_LEDS_SETUP, // We always have 24 leds; don't need this
    COMMS_COMMAND_LEDS_CLEARALL,
    COMMS_COMMAND_LEDS_SETALL,
    COMMS_COMMAND_LEDS_SETONE,
    COMMS_COMMAND_ANIMATION_LOADSTART,
    COMMS_COMMAND_ANIMATION_LOADPATTERN,
    COMMS_COMMAND_ANIMATION_START,
    COMMS_COMMAND_ANIMATION_STOPIMMEDIATE,
    COMMS_COMMAND_ANIMATION_STOPATEND,
    COMMS_COMMAND_ANIMATION_RESUME,
    COMMS_COMMAND_INVALID,
} CommsCommand_t;

typedef enum {
    COMMS_RESPONSE_STATUS = 0,
    COMMS_RESPONSE_VERSION,
    COMMS_RESPONSE_SLIDER,
    COMMS_RESPONSE_BUTTON,
    COMMS_RESPONSE_ANIMATION_STARTED,
    COMMS_RESPONSE_ANIMATION_STOPPED,
} CommsResponse_t;

#define COMMS_STATUS_FAILURE 0
#define COMMS_STATUS_SUCCESS 1

#define COMMS_TX_BUFFER_SIZE 6 // Outgoing buffer size is fixed

#endif /* COMMS_H_ */
