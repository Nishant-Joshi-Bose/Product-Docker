/*******************************************************************************
* File Name: Status.h  
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

#if !defined(CY_PINS_Status_ALIASES_H) /* Pins Status_ALIASES_H */
#define CY_PINS_Status_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Status_0			(Status__0__PC)
#define Status_0_PS		(Status__0__PS)
#define Status_0_PC		(Status__0__PC)
#define Status_0_DR		(Status__0__DR)
#define Status_0_SHIFT	(Status__0__SHIFT)
#define Status_0_INTR	((uint16)((uint16)0x0003u << (Status__0__SHIFT*2u)))

#define Status_INTR_ALL	 ((uint16)(Status_0_INTR))


#endif /* End Pins Status_ALIASES_H */


/* [] END OF FILE */
