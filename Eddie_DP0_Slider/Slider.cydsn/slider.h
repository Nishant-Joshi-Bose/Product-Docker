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

#ifndef SLIDER_CYDSN_SLIDER_H_
#define SLIDER_CYDSN_SLIDER_H_

typedef enum {
    SLIDER_DOWN = 0,
    SLIDER_UP,
    SLIDER_MOVE,
} SliderState_t;

void SlidersInit(void);
void SlidersScan(void);

#endif /* SLIDER_CYDSN_SLIDER_H_ */
