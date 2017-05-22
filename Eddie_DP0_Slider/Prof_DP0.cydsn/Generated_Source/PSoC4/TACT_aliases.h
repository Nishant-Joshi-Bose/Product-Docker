/*******************************************************************************
* File Name: TACT.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_TACT_ALIASES_H) /* Pins TACT_ALIASES_H */
#define CY_PINS_TACT_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define TACT_0			(TACT__0__PC)
#define TACT_0_PS		(TACT__0__PS)
#define TACT_0_PC		(TACT__0__PC)
#define TACT_0_DR		(TACT__0__DR)
#define TACT_0_SHIFT	(TACT__0__SHIFT)
#define TACT_0_INTR	((uint16)((uint16)0x0003u << (TACT__0__SHIFT*2u)))

#define TACT_INTR_ALL	 ((uint16)(TACT_0_INTR))


#endif /* End Pins TACT_ALIASES_H */


/* [] END OF FILE */
