/**
 * @file slider.h
 * @author 
 *
 * 
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
