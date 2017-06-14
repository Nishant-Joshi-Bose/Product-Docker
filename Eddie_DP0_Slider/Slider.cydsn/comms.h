/**
 * @file comms.h
 * @author 
 *
 * 
 */

#ifndef COMMS_H_
#define COMMS_H_

#include <project.h>
#include "util.h"

// TODO remove any non-global funcs and static-ize them in the c file
void CommsInit(void);
BOOL CommsIsInputBufferReady(void);
uint8 *CommsGetInputBuffer(void);
void CommsSendData(const uint8_t *buffer);
void CommsResetInputBuffer(void);
void CommsHandleIncoming(void);
void CommsSendStatus(BOOL status);

typedef enum {
    COMMS_COMMAND_GETVERSION = 0,
    COMMS_COMMAND_LEDS_SETUP,
    COMMS_COMMAND_LEDS_CLEARALL,
    COMMS_COMMAND_LEDS_SETALL,
    COMMS_COMMAND_LEDS_SETONE,
    COMMS_COMMAND_SENSOR_ENABLE,
    COMMS_COMMAND_SENSOR_DISABLE,
    COMMS_COMMAND_BUTTONS_SETUP,
    COMMS_COMMAND_SLIDERS_SETUP,
    COMMS_COMMAND_INVALID,
} CommsCommand_t;

typedef enum {
    COMMS_RESPONSE_STATUS = 0,
    COMMS_RESPONSE_SLIDER,
    COMMS_RESPONSE_BUTTON,
} CommsResponse_t;

#define COMMS_STATUS_FAILURE 0
#define COMMS_STATUS_SUCCESS 1

#endif /* COMMS_H_ */
