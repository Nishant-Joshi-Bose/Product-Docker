/**
 * @file comms.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"
#include "comms.h"
#include "led.h"
#include "button.h"
#include "slider.h"
#include "capsensehdlr.h"

#define I2C_RCV_BUFFER_SIZE 64 // Max incoming buffer is around 48
#define I2C_TX_BUFFER_SIZE 4 // Outgoing buffer is always 4
static uint8 i2cRxBuffer[I2C_RCV_BUFFER_SIZE];
static uint8 i2cTxBuffer[I2C_TX_BUFFER_SIZE];
#define I2C_MASTER_READ_TIMEOUT 16 // 1s/16 timer interrupt ticks

void CommsInit(void)
{
    I2CS_I2CSlaveInitReadBuf (i2cTxBuffer,  I2C_TX_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cRxBuffer, I2C_RCV_BUFFER_SIZE);
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

// TODO maybe circular buffer and batch up instead of turning off capsense but we may not have the ram
void CommsSendData(const uint8_t *buffer)
{
    memcpy(i2cTxBuffer, buffer, I2C_TX_BUFFER_SIZE);
    uint_fast64_t startTime = get_timer_interrrupt_count();
    // Interrupt the client to let it know it has to read now
    CAPINT_Write(1u);
    // Wait until master is done reading
    while (0u == (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_RD_CMPLT))
    {
        // Timeout here in case there's a failure
        if (get_timer_interrrupt_count() > startTime+I2C_MASTER_READ_TIMEOUT)
        {
            break; // the master will have to deal with garbled stuff since they bagged out of reading in the first place
        }
    }
    /* Clear slave read buffer and status */
    I2CS_I2CSlaveClearReadBuf();
    (void) I2CS_I2CSlaveClearReadStatus();
    // Reset client interrupt
    CAPINT_Write(0u);
}

void CommsSendStatus(BOOL success)
{
}

static void CommsSendVersion(void)
{

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
        case COMMS_COMMAND_LEDS_SETUP:
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
        case COMMS_COMMAND_BUTTONS_SETUP:
            CommsSendStatus(ButtonsSetup(buff));
            break;
        case COMMS_COMMAND_SLIDERS_SETUP:
            CommsSendStatus(SlidersSetup(buff));
            break;
        // NO DEFAULT! if we add commands we want the compiler to barf if we forget to check here
        }

        CommsResetInputBuffer();
    }
}
