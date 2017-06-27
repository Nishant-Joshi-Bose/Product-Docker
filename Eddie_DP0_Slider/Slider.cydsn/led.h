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

#ifndef LED_H
#define LED_H

#include <project.h>
#include "util.h"

#define MAX_LEDS               24 

void LedsInit(void);
BOOL LedsHandleCommand(const uint8_t *buff);
void LedsShowPattern(uint8_t *pattern);

#endif // LED_H
/* [] END OF FILE */
