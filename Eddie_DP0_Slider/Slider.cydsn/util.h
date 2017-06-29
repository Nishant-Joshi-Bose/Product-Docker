/*
 * @file
 *
 * @brief
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#ifndef UTIL_H
#define UTIL_H
    
#include <project.h>
    
#ifndef BOOL
#define BOOL uint8
#endif // BOOL

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif // TRUE

#define USE_TUNER                    0 // will enable the Cypress capsense tuner

uint8_t get_software_version_major(void);
uint8_t get_software_version_minor(void);

#ifndef NDEBUG
void DebugTell(uint16_t data);
#endif

#endif // UTIL_H
