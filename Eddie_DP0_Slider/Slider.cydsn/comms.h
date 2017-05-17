/**
 * @file comms.h
 * @author 
 *
 * 
 */

#ifndef COMMS_H_
#define COMMS_H_

void CommsInit(void);
BOOL CommsIsInputBufferReady(void);
char *CommsGetInputBuffer(void);
void CommsSendData(const char *buffer);
void CommsResetInputBuffer(void);

#endif /* COMMS_H_ */
