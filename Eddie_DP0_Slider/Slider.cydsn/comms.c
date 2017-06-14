/**
 * @file comms.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"

#define I2C_BUFFER_SIZE 96 // Bootloader uses about 64; just be safe
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

void CommsSendData(uint8_t count, const uint8_t *buffer)
{
    memcpy(i2cTxBuffer, buffer, count);
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

void CommsHandleIncoming(void)
{
    if (CommsIsInputBufferReady())
    {
        if (get_is_telemetry_enable() == FALSE)
        {
            set_is_telemetry_enable(TRUE);
        }// the telemetry were disabled but we received something from the client

        char *receive_string = (char *)CommsGetInputBuffer();

        if ((receive_string[0] != START_OF_CMD) || (receive_string[strlen(receive_string) - 1] != END_OF_CMD))
        {
            send_alarm_telemetry(ALARM_WARNING, "", "invalid start-end character(s)");
        }// if the command is not well formed
        else
        {
            receive_string[strlen(receive_string) - 1] = '\0';
            if (parse_and_execute_command(&(receive_string[1])) == FALSE)
            {
                send_alarm_telemetry(ALARM_WARNING, command, "failed to execute the command");
            }// If we failed to execute the command
            else
            {
                send_alarm_telemetry(ALARM_LOG, command, "executed");
            }
        }// else, the command seems well formed

        CommsResetInputBuffer();
    }
}
