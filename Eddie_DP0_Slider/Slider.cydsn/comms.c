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
static uint8 i2cTxBuffer[COMMS_TX_BUFFER_SIZE];
#define I2C_MASTER_READ_TIMEOUT 16 // 1s/16 timer interrupt ticks

void CommsInit(void)
{
    I2CS_I2CSlaveInitReadBuf (i2cTxBuffer,  COMMS_TX_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cRxBuffer, COMMS_RCV_BUFFER_SIZE);
    I2CS_Start();
}

BOOL CommsIsInputBufferReady(void)
{
    return (0u != (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_WR_CMPLT));
}

void CommsResetInputBuffer(void)
{
    I2CS_I2CSlaveClearWriteBuf();
    (void) I2CS_I2CSlaveClearWriteStatus();
}

uint8 *CommsGetInputBuffer(void)
{
    return(i2cRxBuffer);
}

// TODO maybe circular buffer and batch up if we have the ram for it
void CommsSendData(const uint8_t *buffer)
{
    memcpy(i2cTxBuffer, buffer, COMMS_TX_BUFFER_SIZE);
//    uint_fast64_t startTime = get_timer_interrrupt_count();
    // Interrupt the client to let it know it has to read now
    CAPINT_Write(1u);

    uint32_t timeout = 0x100000;
    // Wait until master is done reading
    while (0u == (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_RD_CMPLT))
    {
        timeout--;
        if (timeout == 0)
        {
            break;
        }
        // Timeout here in case there's a failure
//        if (get_timer_interrrupt_count() > startTime+I2C_MASTER_READ_TIMEOUT)
//        {
//            break; // the master will have to deal with garbled stuff since they bagged out of reading in the first place
//        }
    }
    /* Clear slave read buffer and status */
    I2CS_I2CSlaveClearReadBuf();
    (void) I2CS_I2CSlaveClearReadStatus();
    // Reset client interrupt
    CAPINT_Write(0u);
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

void CommsHandleIncoming(void)
{
    if (CommsIsInputBufferReady())
    {
        uint8_t *buff = CommsGetInputBuffer();

        if (buff[0] >= COMMS_COMMAND_INVALID)
        {
            CommsSendStatus(COMMS_STATUS_FAILURE);
            return;
        }

        switch ((CommsCommand_t) buff[0])
        {
        case COMMS_COMMAND_GETVERSION:
            CommsSendVersion();
            break;
//        case COMMS_COMMAND_LEDS_SETUP:
        case COMMS_COMMAND_LEDS_CLEARALL:
        case COMMS_COMMAND_LEDS_SETALL:
        case COMMS_COMMAND_LEDS_SETONE:
            CommsSendStatus(LedsHandleCommand(buff));
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
            AnimationHandleCommand(buff);
            break;
        case COMMS_COMMAND_INVALID:
            CommsSendStatus(COMMS_STATUS_FAILURE);
            break;
        // NO DEFAULT! if we add commands we want the compiler to barf if we forget to check here
        }

        CommsResetInputBuffer();
    }
}
