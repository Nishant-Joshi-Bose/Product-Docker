/*******************************************************************************
* File Name: communication_api.c  
* Version 1.0
*
* Description:
* This file is created by the author . This contains definitions of APIs 
* used in structure 'CyBtldr_CommunicationsData' defined in cybtldr_api.h ,
* using I2C commuincations component 
********************************************************************************/

#include "communication_api.h"

/*******************************************************************************
* Function Name: OpenConnection
********************************************************************************
*
* Summary:
*  Initializes the communications component : In this case I2C
*
* Parameters:  
*  void
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int OpenConnection(void)
{
	I2C_Start();
	
	/* Clear any previous status */
	I2C_I2CMasterClearStatus();
	
	/* Interrrupt has ro be enbled for the I2C to work properly */
	I2C_EnableInt();
	
	
	return(CYRET_SUCCESS);
}


/*******************************************************************************
* Function Name: CloseConnection
********************************************************************************
*
* Summary:
*  Clears the status and stops the communications component(I2C).
*
* Parameters:  
*  void
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int CloseConnection(void)
{
	/* Clear any previous status */
	I2C_I2CMasterClearStatus(); 

    I2C_DisableInt();
	
	I2C_Stop();

	return(CYRET_SUCCESS);
}

/*******************************************************************************
* Function Name: WriteData
********************************************************************************
*
* Summary:
*  Writes the specified number of bytes usign the communications component(I2C)
*
* Parameters:  
*  wrData - Pointer to write data buffer
*  byteCnt - No. of bytes to be written 
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int WriteData(unsigned char* wrData, int byteCnt)
{
	unsigned long timeOut =1;

	/*Clear I2C write buffer */
    I2C_I2CMasterClearWriteBuf();
	
	/*Clear the I2C status */
    I2C_I2CMasterClearStatus();

	/* Write data - Refer I2C.h file for details of this API*/	
	I2C_I2CMasterWriteBuf(SLAVE_ADDR,wrData,byteCnt,I2C_I2C_MODE_COMPLETE_XFER);

 	/* Wait till read operation is complete or timeout  */
	while (!(I2C_I2CMasterStatus()&I2C_I2C_MSTAT_WR_CMPLT))
	{
		timeOut++	;
		/* Check for timeout and if so exit with communication error code*/
		if(timeOut == 0)
		{
			return(CYRET_ERR_COMM_MASK);
		}	
	}
	/* Give some delay before the next operation so that the slave can process the written data */
	CyDelay(25);

	return(CYRET_SUCCESS);
	
}


/*******************************************************************************
* Function Name: ReadData
********************************************************************************
*
* Summary:
*  Reads the specified number of bytes usign the communications component(I2C)
*
* Parameters:  
*  rdData - Pointer to read data buffer
*  byteCnt - No. of bytes to be read 
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int ReadData(unsigned char* rdData, int byteCnt)
{
	unsigned short timeOut =1;
	
	/*Clear I2C read buffer */
    I2C_I2CMasterClearReadBuf();
	
	/*Clear I2C status */
    I2C_I2CMasterClearStatus();
	
	/* Read data from I2C slave- Refer I2C.h file for details of this API*/	
	I2C_I2CMasterReadBuf(SLAVE_ADDR,rdData,byteCnt,I2C_I2C_MODE_COMPLETE_XFER);	
 
 	/* Wait till read operation is complete or timeout  */
	while (!(I2C_I2CMasterStatus()&I2C_I2C_MSTAT_RD_CMPLT))
	{
		timeOut++	;

		/* Check for timeout and if so exit with communication error code*/
		if(timeOut == 0)
		{
			return(CYRET_ERR_COMM_MASK);
		}
	}
	
	return(CYRET_SUCCESS);
}

/* [] END OF FILE */

