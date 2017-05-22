/*******************************************************************************
* File Name: Blank.h  
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

#if !defined(CY_PINS_Blank_ALIASES_H) /* Pins Blank_ALIASES_H */
#define CY_PINS_Blank_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Blank_0			(Blank__0__PC)
#define Blank_0_PS		(Blank__0__PS)
#define Blank_0_PC		(Blank__0__PC)
#define Blank_0_DR		(Blank__0__DR)
#define Blank_0_SHIFT	(Blank__0__SHIFT)
#define Blank_0_INTR	((uint16)((uint16)0x0003u << (Blank__0__SHIFT*2u)))

#define Blank_INTR_ALL	 ((uint16)(Blank_0_INTR))


#endif /* End Pins Blank_ALIASES_H */


/* [] END OF FILE */
