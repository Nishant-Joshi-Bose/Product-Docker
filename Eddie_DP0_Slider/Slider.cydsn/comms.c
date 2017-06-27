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

/*
 * Comms protocol
@startuml
title PSoC API
note "Every command sends back its response asynchronously.\nEvery command except GetVersion sends back a StatusResponse\nIntensities are 12-bit values sent as 16-bits" as n1
package Commands {

class StatusResponse << (R, #FF7700) >> {
0x00
..
status: Fail = 0, Success = 1
}

class GetVersion {
0x00
}
class VersionResponse << (R, #FF7700) >> {
0x01
..
version-major
version-minor
}

StatusResponse -[hidden]down-> GetVersion

GetVersion -[hidden]down-> VersionResponse

class LEDsClearAll {
0x01
}

VersionResponse -[hidden]down-> LEDsClearAll

class LEDsSetAll {
0x02
..
0x00: UNUSED
0x0xyz: 12-bit intensity 1
...
0x0xyz: 12-bit intensity 24
}

LEDsClearAll -[hidden]down-> LEDsSetAll

class LEDsSetOne {
0x03
..
led: 1 byte
0x0xyz: 12-bit intensity
}
}

LEDsSetAll -[hidden]down-> LEDsSetOne

package Events {
class SliderEvent << (R, #FF7700) >> {
0x02
..
slider id: 1 byte
slider position: 16 bits
state: 1 byte; down=0, up=1, move=2
}

class ButtonEvent << (R, #FF7700) >> {
0x03
..
button id: 1 byte
pressed: 1 byte; pressed=1, released=0
}

SliderEvent -[hidden]down-> ButtonEvent
}

n1 --[hidden]down-> Commands
Commands -[hidden]down-> Events
@enduml

 */

#include <project.h>
#include "util.h"
#include "comms.h"
#include "led.h"
#include "button.h"
#include "slider.h"
#include "capsensehdlr.h"
#include "animation.h"

#define COMMS_RCV_BUFFER_SIZE 64 // Max incoming buffer is around 49
static uint8 i2cRxBuffer[COMMS_RCV_BUFFER_SIZE];
static uint8 i2cRxBufferCopy[COMMS_RCV_BUFFER_SIZE];

// Circular buffer for outgoing comms
#define TX_QUEUE_SIZE 64
static uint8 i2cTxBuffer[TX_QUEUE_SIZE * COMMS_TX_BUFFER_SIZE];
// What we've sent
static uint8 i2cTxSentIdx = 0;
// What's still left to send
static uint8 i2cTxToSendIdx = 0;

#define MASTER_INTERRUPT_TIMEOUT_DEFAULT 0x10000
static uint32_t readTimeout = MASTER_INTERRUPT_TIMEOUT_DEFAULT;

void CommsInit(void)
{
    I2CS_I2CSlaveInitReadBuf (i2cTxBuffer,  COMMS_TX_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cRxBuffer, COMMS_RCV_BUFFER_SIZE);
    I2CS_Start();
}

void CommsSendData(const uint8 *buff)
{
    // Rather than let someone poop directly into the tx buffer, we have them send
    // their own buffer in that we copy.  This way we don't have to worry about
    // any kind of race conditions between any interrupts and user code that may
    // call this.
    uint8 interruptState = CyEnterCriticalSection();
    memcpy(&i2cTxBuffer[i2cTxToSendIdx * COMMS_TX_BUFFER_SIZE], buff, COMMS_TX_BUFFER_SIZE);
    i2cTxToSendIdx++;
    // Circular buffer, rotate
    if (i2cTxToSendIdx >= TX_QUEUE_SIZE)
    {
        i2cTxToSendIdx = 0;
    }
    CyExitCriticalSection(interruptState);
}

static void CommsHandleSend(void)
{
    volatile uint32 i2cStatus = I2CS_I2CSlaveStatus();

    // If reading, just let it continue reading
    if (0u != (i2cStatus & I2CS_I2C_SSTAT_RD_BUSY))
    {
        return;
    }

    // Handle done reading
    if (0u != (i2cStatus & I2CS_I2C_SSTAT_RD_CMPLT))
    {
        // Clear master interrupt
        CAPINT_Write(0u);

        (void) I2CS_I2CSlaveClearReadStatus();

        uint8 interruptState = CyEnterCriticalSection();
        i2cTxSentIdx++;
        if (i2cTxSentIdx >= TX_QUEUE_SIZE)
        {
            i2cTxSentIdx = 0;
        }

        // Re-point the outgoing i2c buffer to the next thing to send
        I2CS_I2CSlaveInitReadBuf (&i2cTxBuffer[i2cTxSentIdx * COMMS_TX_BUFFER_SIZE],  COMMS_TX_BUFFER_SIZE);

        CyExitCriticalSection(interruptState);
    }

    // If there's stuff left to send, interrupt the master
    if (i2cTxSentIdx != i2cTxToSendIdx)
    {
        // If we're already interrupting the master, give it some time to start the read
        if (CAPINT_Read() == 1u)
        {
            readTimeout--;
            if (readTimeout != 0)
            {
                return;
            }
        }

        readTimeout = MASTER_INTERRUPT_TIMEOUT_DEFAULT;
        CAPINT_Write(0u); // In case it was set before and the master ignored it/timed out
        CAPINT_Write(1u);
    }
}

void CommsSendStatus(BOOL status)
{
    uint8_t buff[COMMS_TX_BUFFER_SIZE];

    memset(buff, 0, sizeof(buff));
    buff[0] = COMMS_RESPONSE_STATUS;
    buff[1] = status;

    CommsSendData(buff);
}

static void CommsSendVersion(void)
{
    uint8_t buff[COMMS_TX_BUFFER_SIZE];

    memset(buff, 0, sizeof(buff));
    buff[0] = COMMS_RESPONSE_VERSION;
    buff[1] = get_software_version_major();
    buff[2] = get_software_version_minor();

    CommsSendData(buff);
}

void CommsHandler(void)
{
    CommsHandleSend();

    if (0u != (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_WR_CMPLT))
    {
        // When i2c incoming is done we copy the buffer so that i2c can continue receiving
        // in the background (its interrupt routine) rather than stall it to process the command
        uint8 interruptState = CyEnterCriticalSection();
        memcpy(i2cRxBufferCopy, i2cRxBuffer, COMMS_RCV_BUFFER_SIZE);
        I2CS_I2CSlaveClearWriteBuf();
        CyExitCriticalSection(interruptState);
        I2CS_I2CSlaveClearWriteStatus();

        // Process incoming commands
        if (i2cRxBufferCopy[0] >= COMMS_COMMAND_INVALID)
        {
            CommsSendStatus(COMMS_STATUS_FAILURE);
            return;
        }

        switch ((CommsCommand_t) i2cRxBufferCopy[0])
        {
        case COMMS_COMMAND_GETVERSION:
            CommsSendVersion();
            break;
        case COMMS_COMMAND_LEDS_CLEARALL:
        case COMMS_COMMAND_LEDS_SETALL:
        case COMMS_COMMAND_LEDS_SETONE:
            CommsSendStatus(LedsHandleCommand(i2cRxBufferCopy));
            break;
        case COMMS_COMMAND_ANIMATION_LOADSTART:
        case COMMS_COMMAND_ANIMATION_LOADPATTERN:
        case COMMS_COMMAND_ANIMATION_START:
        case COMMS_COMMAND_ANIMATION_STOPATEND:
        case COMMS_COMMAND_ANIMATION_STOPIMMEDIATE:
        case COMMS_COMMAND_ANIMATION_RESUME:
            AnimationHandleCommand(i2cRxBufferCopy);
            break;
        case COMMS_COMMAND_INVALID:
            CommsSendStatus(COMMS_STATUS_FAILURE);
            break;
        // NO DEFAULT! if we add commands we want the compiler to barf if we forget to check here
        }
    }
}
