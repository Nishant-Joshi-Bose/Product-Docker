/*******************************************************************************
* This file is automatically generated by PSoC Creator
* and should not be edited by hand.
*
* This file is necessary for your project to build.
* Please do not delete it.
********************************************************************************
* Copyright 2008-2011, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#ifndef DEVICE_H
#define DEVICE_H
#include <project.h>

/* This function bootloads the .cyacd file. It sends command packets and flash data to the target.
   Based on the response from the target, it decides whether to continue bootloading. */
unsigned char BootloadStringImage(const char *bootloadImagePtr[],unsigned int lineCount);

#endif
/* [] END OF FILE */
