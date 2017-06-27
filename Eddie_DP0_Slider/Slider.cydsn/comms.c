/**
 * @file comms.c
 * @author 
 *
 * 
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
version
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
#define TX_QUEUE_SIZE 24
static uint8 i2cTxBuffer[TX_QUEUE_SIZE * COMMS_TX_BUFFER_SIZE];
// What we've sent
static uint8 i2cTxSentIdx = 0;
// What's still left to send
static uint8 i2cTxToSendIdx = 0;

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
    i2cTxToSendIdx += COMMS_TX_BUFFER_SIZE;
    // Circular buffer, rotate
    if (i2cTxToSendIdx >= COMMS_TX_BUFFER_SIZE * TX_QUEUE_SIZE)
    {
        i2cTxToSendIdx = 0;
    }
    CyExitCriticalSection(interruptState);
    // Interrupt the master to let it know it has to read now
    CAPINT_Write(0u);
    CAPINT_Write(1u);
}

static void CommsHandleSent(void)
{
    if (0u == (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_RD_CMPLT))
    {
        // TODO come up with a way to do timeout here
        // not sure what we would do with the outbound queue
        return;
    }

    // Clear master interrupt
    CAPINT_Write(0u);

    (void) I2CS_I2CSlaveClearReadStatus();

    uint8 interruptState = CyEnterCriticalSection();
    i2cTxSentIdx += COMMS_TX_BUFFER_SIZE;
    if (i2cTxSentIdx >= COMMS_TX_BUFFER_SIZE * TX_QUEUE_SIZE)
    {
        i2cTxSentIdx = 0;
    }

    // Re-point the outgoing i2c buffer to the next thing to send
    I2CS_I2CSlaveInitReadBuf (&i2cTxBuffer[i2cTxSentIdx * COMMS_TX_BUFFER_SIZE],  COMMS_TX_BUFFER_SIZE);

    // If there's more to send, interrupt the master again
    if (i2cTxSentIdx != i2cTxToSendIdx)
    {
        CAPINT_Write(1u);
    }
    CyExitCriticalSection(interruptState);
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
    buff[1] = get_software_version();

    CommsSendData(buff);
}

void CommsHandler(void)
{
    CommsHandleSent();

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
        case COMMS_COMMAND_SENSOR_ENABLE:
            CommsSendStatus(CapsenseHandlerEnable());
            break;
        case COMMS_COMMAND_SENSOR_DISABLE:
            CommsSendStatus(CapsenseHandlerDisable());
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
