/*
 * comms.c
 *
 *  Created on: May 31, 2017
 *      Author: mike
 */

#include "stm32f3xx_hal.h"
#include "cybtldr_utils.h"
#include "comms.h"

I2C_HandleTypeDef hi2c1;

void CommsInit(void)
{
      hi2c1.Instance = I2C1;
      hi2c1.Init.Timing = 0x0000020B;
      hi2c1.Init.OwnAddress1 = 0x13;
      hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
      hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
      hi2c1.Init.OwnAddress2 = 0;
      hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
      hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
      hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
      if (HAL_I2C_Init(&hi2c1) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

        /**Configure Analogue filter
        */
      if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

        /**Configure Digital filter
        */
      if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }
}

int CommsOpenConnection(void)
{
    // Does nothing right now
    return CYRET_SUCCESS;
}

int CommsCloseConnection(void)
{
    // Does nothing right now
    return CYRET_SUCCESS;
}

int CommsReadData(unsigned char* rdData, int byteCnt)
{
    if(HAL_I2C_Master_Receive(&hi2c1, (uint16_t)PSOC_SLAVE_ADDRESS, (uint8_t *)rdData, byteCnt, 10000) != HAL_OK)
    {
      /* Error_Handler() function is called when Timeout error occurs.
         When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
      if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
      {
          return CYRET_ERR_UNK;
      }
    }
    HAL_Delay(25);
    return CYRET_SUCCESS;
}

int CommsWriteData(unsigned char* wrData, int byteCnt)
{
    if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)PSOC_SLAVE_ADDRESS, (uint8_t*)wrData, byteCnt, 10000)!= HAL_OK)
    {
      /* Error_Handler() function is called when Timeout error occurs.
         When Acknowledge failure occurs (Slave don't acknowledge its address)
         Master restarts communication */
      if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
      {
          return CYRET_ERR_UNK;
      }
    }
    // PSoC example for this says to delay 25ms to allow the slave to process; not sure we wanna do that
    HAL_Delay(25);
    return CYRET_SUCCESS;
}
