/*******************************************************************************
* File Name: cap_int.h  
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

#if !defined(CY_PINS_cap_int_ALIASES_H) /* Pins cap_int_ALIASES_H */
#define CY_PINS_cap_int_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define cap_int_0			(cap_int__0__PC)
#define cap_int_0_PS		(cap_int__0__PS)
#define cap_int_0_PC		(cap_int__0__PC)
#define cap_int_0_DR		(cap_int__0__DR)
#define cap_int_0_SHIFT	(cap_int__0__SHIFT)
#define cap_int_0_INTR	((uint16)((uint16)0x0003u << (cap_int__0__SHIFT*2u)))

#define cap_int_INTR_ALL	 ((uint16)(cap_int_0_INTR))


#endif /* End Pins cap_int_ALIASES_H */


/* [] END OF FILE */
