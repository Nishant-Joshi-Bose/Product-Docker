/*******************************************************************************
* File Name: LED_Driver_SCBCLK.h
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

#if !defined(CY_CLOCK_LED_Driver_SCBCLK_H)
#define CY_CLOCK_LED_Driver_SCBCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void LED_Driver_SCBCLK_StartEx(uint32 alignClkDiv);
#define LED_Driver_SCBCLK_Start() \
    LED_Driver_SCBCLK_StartEx(LED_Driver_SCBCLK__PA_DIV_ID)

#else

void LED_Driver_SCBCLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void LED_Driver_SCBCLK_Stop(void);

void LED_Driver_SCBCLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 LED_Driver_SCBCLK_GetDividerRegister(void);
uint8  LED_Driver_SCBCLK_GetFractionalDividerRegister(void);

#define LED_Driver_SCBCLK_Enable()                         LED_Driver_SCBCLK_Start()
#define LED_Driver_SCBCLK_Disable()                        LED_Driver_SCBCLK_Stop()
#define LED_Driver_SCBCLK_SetDividerRegister(clkDivider, reset)  \
    LED_Driver_SCBCLK_SetFractionalDividerRegister((clkDivider), 0u)
#define LED_Driver_SCBCLK_SetDivider(clkDivider)           LED_Driver_SCBCLK_SetDividerRegister((clkDivider), 1u)
#define LED_Driver_SCBCLK_SetDividerValue(clkDivider)      LED_Driver_SCBCLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define LED_Driver_SCBCLK_DIV_ID     LED_Driver_SCBCLK__DIV_ID

#define LED_Driver_SCBCLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define LED_Driver_SCBCLK_CTRL_REG   (*(reg32 *)LED_Driver_SCBCLK__CTRL_REGISTER)
#define LED_Driver_SCBCLK_DIV_REG    (*(reg32 *)LED_Driver_SCBCLK__DIV_REGISTER)

#define LED_Driver_SCBCLK_CMD_DIV_SHIFT          (0u)
#define LED_Driver_SCBCLK_CMD_PA_DIV_SHIFT       (8u)
#define LED_Driver_SCBCLK_CMD_DISABLE_SHIFT      (30u)
#define LED_Driver_SCBCLK_CMD_ENABLE_SHIFT       (31u)

#define LED_Driver_SCBCLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << LED_Driver_SCBCLK_CMD_DISABLE_SHIFT))
#define LED_Driver_SCBCLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << LED_Driver_SCBCLK_CMD_ENABLE_SHIFT))

#define LED_Driver_SCBCLK_DIV_FRAC_MASK  (0x000000F8u)
#define LED_Driver_SCBCLK_DIV_FRAC_SHIFT (3u)
#define LED_Driver_SCBCLK_DIV_INT_MASK   (0xFFFFFF00u)
#define LED_Driver_SCBCLK_DIV_INT_SHIFT  (8u)

#else 

#define LED_Driver_SCBCLK_DIV_REG        (*(reg32 *)LED_Driver_SCBCLK__REGISTER)
#define LED_Driver_SCBCLK_ENABLE_REG     LED_Driver_SCBCLK_DIV_REG
#define LED_Driver_SCBCLK_DIV_FRAC_MASK  LED_Driver_SCBCLK__FRAC_MASK
#define LED_Driver_SCBCLK_DIV_FRAC_SHIFT (16u)
#define LED_Driver_SCBCLK_DIV_INT_MASK   LED_Driver_SCBCLK__DIVIDER_MASK
#define LED_Driver_SCBCLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_LED_Driver_SCBCLK_H) */

/* [] END OF FILE */
