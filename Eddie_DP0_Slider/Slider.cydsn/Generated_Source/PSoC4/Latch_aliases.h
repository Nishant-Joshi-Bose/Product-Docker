/*******************************************************************************
* File Name: Latch.h  
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

#if !defined(CY_PINS_Latch_ALIASES_H) /* Pins Latch_ALIASES_H */
#define CY_PINS_Latch_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Latch_0			(Latch__0__PC)
#define Latch_0_PS		(Latch__0__PS)
#define Latch_0_PC		(Latch__0__PC)
#define Latch_0_DR		(Latch__0__DR)
#define Latch_0_SHIFT	(Latch__0__SHIFT)
#define Latch_0_INTR	((uint16)((uint16)0x0003u << (Latch__0__SHIFT*2u)))

#define Latch_INTR_ALL	 ((uint16)(Latch_0_INTR))


#endif /* End Pins Latch_ALIASES_H */


/* [] END OF FILE */
