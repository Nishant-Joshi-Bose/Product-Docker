/**
 * @file comms.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"

#define I2C_BUFFER_SIZE 512
static uint8 i2cRxBuffer[I2C_BUFFER_SIZE];
static uint8 i2cTxBuffer[I2C_BUFFER_SIZE];
#define I2C_MASTER_READ_TIMEOUT 16 // 1s/16 timer interrupt ticks

void CommsInit(void)
{
    I2CS_I2CSlaveInitReadBuf (i2cTxBuffer,  I2C_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cRxBuffer, I2C_BUFFER_SIZE);
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

void CommsSendData(const char *buffer)
{
    memcpy(i2cTxBuffer, buffer, strlen(buffer));
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
