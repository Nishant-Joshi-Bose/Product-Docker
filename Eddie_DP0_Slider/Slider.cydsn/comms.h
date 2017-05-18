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
void CommsSendData(const char *buffer);
void CommsResetInputBuffer(void);

#endif /* COMMS_H_ */
