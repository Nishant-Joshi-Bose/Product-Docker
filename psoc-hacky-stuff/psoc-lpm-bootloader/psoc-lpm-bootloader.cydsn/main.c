/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

#define I2C_BUFFER_SIZE 128
static uint8 i2cRxBuffer[I2C_BUFFER_SIZE];
static uint8 i2cTxBuffer[I2C_BUFFER_SIZE];
#define I2C_MASTER_READ_TIMEOUT 16 // 1s/16 timer interrupt ticks

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2CS_I2CSlaveInitReadBuf (i2cTxBuffer,  I2C_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cRxBuffer, I2C_BUFFER_SIZE);
    I2CS_Start();

    for(;;)
    {
        while (0u == (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_WR_CMPLT));
        I2CS_I2CSlaveClearWriteBuf();
        (void) I2CS_I2CSlaveClearWriteStatus();
        memcpy(i2cTxBuffer, i2cRxBuffer, I2C_BUFFER_SIZE);
        while (0u == (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_RD_CMPLT));
        I2CS_I2CSlaveClearReadBuf();
        (void) I2CS_I2CSlaveClearReadStatus();
    }
}

/* [] END OF FILE */
