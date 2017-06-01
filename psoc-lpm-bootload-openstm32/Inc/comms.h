/*
 * comms.h
 *
 *  Created on: May 31, 2017
 *      Author: mike
 */

#ifndef COMMS_H_
#define COMMS_H_

#define PSOC_SLAVE_ADDRESS 0x10 // STM32 I2C uses 8 bit addresses

void CommsInit(void);
int CommsOpenConnection(void);
int CommsCloseConnection(void);
int CommsReadData(unsigned char* rdData, int byteCnt);
int CommsWriteData(unsigned char* wrData, int byteCnt);

#endif /* COMMS_H_ */
