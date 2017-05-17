/**
 * @file comms.c
 * @author 
 *
 * 
 */

#include <project.h>

#define I2C_BUFFER_SIZE 128
static uint8 i2cRxBuffer[I2C_BUFFER_SIZE];
static uint8 i2cTxBuffer[I2C_BUFFER_SIZE];

void CommsInit(void)
{
    I2CS_I2CSlaveInitReadBuf (i2cRxBuffer,  I2C_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cTxBuffer, I2C_BUFFER_SIZE);
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

char *CommsGetInputBuffer(void)
{
    return(i2cRxBuffer);
}

void CommsSendData(const char *buffer)
{
    memcpy(i2cTxBuffer, buffer, I2C_BUFFER_SIZE);
    // Interrupt the client to let it know it has to read now
//    cap_int_Write(1u);
    // Wait until master is done reading
    while (0u == (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_RD_CMPLT))
    {
        // TODO do a timeout here in case there's a failure
    }
    /* Clear slave read buffer and status */
    I2CS_I2CSlaveClearReadBuf();
    (void) I2CS_I2CSlaveClearReadStatus();
    // Reset client interrupt
//    cap_int_Write(0u);
}
