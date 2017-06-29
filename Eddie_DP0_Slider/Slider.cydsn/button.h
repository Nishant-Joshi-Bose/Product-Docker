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

#ifndef SLIDER_CYDSN_BUTTON_H_
#define SLIDER_CYDSN_BUTTON_H_

#include "project.h"
    
void ButtonsScan(void);
void SendButtonEvent(uint8_t buttonId, uint8_t pressed);

#endif /* SLIDER_CYDSN_BUTTON_H_ */
