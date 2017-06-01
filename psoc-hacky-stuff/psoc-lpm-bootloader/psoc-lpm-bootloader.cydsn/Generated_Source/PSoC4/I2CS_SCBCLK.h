/*******************************************************************************
* File Name: I2CS_SCBCLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_I2CS_SCBCLK_H)
#define CY_CLOCK_I2CS_SCBCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void I2CS_SCBCLK_StartEx(uint32 alignClkDiv);
#define I2CS_SCBCLK_Start() \
    I2CS_SCBCLK_StartEx(I2CS_SCBCLK__PA_DIV_ID)

#else

void I2CS_SCBCLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void I2CS_SCBCLK_Stop(void);

void I2CS_SCBCLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 I2CS_SCBCLK_GetDividerRegister(void);
uint8  I2CS_SCBCLK_GetFractionalDividerRegister(void);

#define I2CS_SCBCLK_Enable()                         I2CS_SCBCLK_Start()
#define I2CS_SCBCLK_Disable()                        I2CS_SCBCLK_Stop()
#define I2CS_SCBCLK_SetDividerRegister(clkDivider, reset)  \
    I2CS_SCBCLK_SetFractionalDividerRegister((clkDivider), 0u)
#define I2CS_SCBCLK_SetDivider(clkDivider)           I2CS_SCBCLK_SetDividerRegister((clkDivider), 1u)
#define I2CS_SCBCLK_SetDividerValue(clkDivider)      I2CS_SCBCLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define I2CS_SCBCLK_DIV_ID     I2CS_SCBCLK__DIV_ID

#define I2CS_SCBCLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define I2CS_SCBCLK_CTRL_REG   (*(reg32 *)I2CS_SCBCLK__CTRL_REGISTER)
#define I2CS_SCBCLK_DIV_REG    (*(reg32 *)I2CS_SCBCLK__DIV_REGISTER)

#define I2CS_SCBCLK_CMD_DIV_SHIFT          (0u)
#define I2CS_SCBCLK_CMD_PA_DIV_SHIFT       (8u)
#define I2CS_SCBCLK_CMD_DISABLE_SHIFT      (30u)
#define I2CS_SCBCLK_CMD_ENABLE_SHIFT       (31u)

#define I2CS_SCBCLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << I2CS_SCBCLK_CMD_DISABLE_SHIFT))
#define I2CS_SCBCLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << I2CS_SCBCLK_CMD_ENABLE_SHIFT))

#define I2CS_SCBCLK_DIV_FRAC_MASK  (0x000000F8u)
#define I2CS_SCBCLK_DIV_FRAC_SHIFT (3u)
#define I2CS_SCBCLK_DIV_INT_MASK   (0xFFFFFF00u)
#define I2CS_SCBCLK_DIV_INT_SHIFT  (8u)

#else 

#define I2CS_SCBCLK_DIV_REG        (*(reg32 *)I2CS_SCBCLK__REGISTER)
#define I2CS_SCBCLK_ENABLE_REG     I2CS_SCBCLK_DIV_REG
#define I2CS_SCBCLK_DIV_FRAC_MASK  I2CS_SCBCLK__FRAC_MASK
#define I2CS_SCBCLK_DIV_FRAC_SHIFT (16u)
#define I2CS_SCBCLK_DIV_INT_MASK   I2CS_SCBCLK__DIVIDER_MASK
#define I2CS_SCBCLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_I2CS_SCBCLK_H) */

/* [] END OF FILE */
