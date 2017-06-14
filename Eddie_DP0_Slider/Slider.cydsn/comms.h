/**
 * @file comms.h
 * @author 
 *
 * 
 */

#ifndef COMMS_H_
#define COMMS_H_

#include <project.h>
#include "util.h"
    
void CommsInit(void);
BOOL CommsIsInputBufferReady(void);
uint8 *CommsGetInputBuffer(void);
void CommsSendData(uint8_t count, const uint8_t *buffer);
void CommsResetInputBuffer(void);
void CommsHandleIncoming(void);

#endif /* COMMS_H_ */
