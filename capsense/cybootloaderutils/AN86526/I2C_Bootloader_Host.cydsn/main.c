/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/*****************************************************************************************************
*  Project Name: I2C_Bootloader_Host
*  Project Revision: 2.00
*  Software Version: PSoC Creator 3.3 SP2
*  Device Tested: CY8C4245AXI-483
*  Compilers Tested: ARM GCC
*  Related Hardware: CY8CKIT-042
****************************************************************************************************/
/****************************************************************************************************
* Project Description:
* This is a sample bootloader host program demonstrating PSoC4 bootloading PSoC4.
* The project is tested using two CY8CKIT-042 with PSoC 4 respectively as Bootloader Host and Target.
* PSoC4 on Target must be programmed with the I2C Bootloader Program attached with the app note.
*
* Connections Required
* Host CY8CKIT-042 (PSoC 4 DVK) :
*  P4[1] - SDA - Should be connected to SDA of Target PSoC4 (P3[1] has been pulled up with 2.2k resistor on CY8CKIT-042).
*  P4[0] - SCL - Should be connected to SCL of Target PSoC4 (P3[0] has been pulled up with 2.2k resistor on CY8CKIT-042). 
*  P0[4] - RX  - Should be connected to Pin 10 on the expansion header J8.
*  P0[5] - TX  - Should be connected to Pin 9 on the expansion header J8.
*  P0.7 is internally connected to SW2 on DVK.
*
* Target CY8CKIT-042 (PSoC 4 DVK) : PSoC4 intially programmed with I2C_Bootloader program.
*  P3[1] - SDA - Connected to SDA of Host PSoC 4.
*  P3[0] - SCL - Connected to SCL of Host PSoC 4.
*  P0.7 is internally connected to SW2 on DVK.
*
* Note that the GNDs of both DVKs should be connected together.
*
* Bootload function is defined in main.c: BootloadStringImage which uses Bootloader Host APIs
* to bootload the contents of .cyacd file.
*
* BootloadStringImage function requires each line of .cyacd to be stored as a seperate string in a string array.
* The total number of lines in the file should also be manually calculated and stored as #define LINE_CNT
* The string image of .cyacd file is stored in StringImage.h file.
*
* The following events happens alternatively on each switch press
* On first switch press it will bootload 'stringImage_0' stored in StringImage.h using BootloadStringImage function . 
* On next switch press it will bootload 'stringImage_1' stored in StringImage.h using BootloadStringImage function .
*
* These bootloadable images congtrol LED blinking with green or blue color.
* Note that in order to enter the bootloader from the application program P0.7 - connected to SW2 on Target
* should be pressed so that the application program running on Target PSoC4 enters Bootloader and is ready to 
* bootload a new application code/data . 
*
* I2C Slave Addr of the bootloader is set to 0x08 in communication_api.h
****************************************************************************************************/

#include <device.h>
#include <cybtldr_parse.h>
#include <cybtldr_command.h>
#include <communication_api.h>
#include <cybtldr_api.h>
#include "StringImage.h"

/* This structure contains function pointers to the four communication layer functions 
   contained in the communication_api.c / .h */
CyBtldr_CommunicationsData comm1;

/* switch_flag is set upon the user input (button on P0 [7]) */
unsigned char switch_flag = 0u;

/* toggle_appcode alternates between the two bootloadable files */
unsigned char toggle_appcode = 0u;

void main()
{
	/* Place your initialization/startup code here (e.g. MyInst_Start()) */
	unsigned char error, hexData;
		
	char strSuccess[2][50] = {{"Bootloaded. Led blinks with green color on Target"}, \
							  {"Bootloaded. Led blinks with blue color on Target"}};
	char strAction[] = {"Press SW2 on Target and Host in sequence to bootload\r\n"};
	char strStatus[] = {"0x00 \r\n"};

	/* Start UART */
	UART_Start();	
	UART_UartPutString("Press P0.7 to bootload applicaiton\r\n");
	
	/* Enable all interrupts */
	CyGlobalIntEnable;
	
	/* Initialize the communication structure element -comm1 */
	comm1.OpenConnection = &OpenConnection;
	comm1.CloseConnection = &CloseConnection;
	comm1.ReadData = &ReadData;
	comm1.WriteData = &WriteData;
	comm1.MaxTransferSize = 64u;
    
    /* Enable Switch ISR */
	ISR_Switch_Start();	

    for(;;)
    {
	    /* If Switch is pressed, the image will be bootloaded */
		if(1u == switch_flag)
		{
			UART_UartPutString("\r\nStart Bootloading....\r\n");
            
			if(0u == toggle_appcode)
			{
				/* If toggle_appcode = 0,the image to be bootloaded is GreenLED */
				/* Call BootloadStringImage function to bootload stringImage_0 application*/		
				error = BootloadStringImage(stringImage_0,LINE_CNT_0 );
			}
			else
			{
				/* If toggle_appcode = 1,the image to be bootloaded is BlueLED */
				/* Call BootloadStringImage function to bootload stringImage_1 application */	 
				error = BootloadStringImage(stringImage_1,LINE_CNT_1 );
			}			
			
			if( error == CYRET_SUCCESS)
			{
				/* Transfer success information to PC */
				UART_UartPutString(strSuccess[toggle_appcode]);
				UART_UartPutString("\r\n");
				UART_UartPutString(strAction);
				
				/*Change 'toggle_appcode' flag  */
				toggle_appcode ^= 1u;		
			}
			else 
			{
				if(error & CYRET_ERR_COMM_MASK) /* Check for comm error*/
				{
					UART_UartPutString("Communicatn Err \r\n");
				}
				else /* Else transfer the bootload error code */
				{
					UART_UartPutString("Bootload Err :");
					hexData = (error & 0xF0u) >> 4;
					strStatus[2] = hexData > 9u ? (hexData - 10u + 'A'):(hexData + '0');
					hexData = error & 0x0Fu;
					strStatus[3] = hexData > 9u ? (hexData - 10u + 'A'):(hexData + '0');
					UART_UartPutString(strStatus);					
				}
			}	
			
			switch_flag = 0u;
		}
		
		/* Delay 1ms */
		CyDelay(1u);
	   
    }
}


/****************************************************************************************************
* Function Name: BootloadStringImage
*****************************************************************************************************
*
* Summary:
*  Bootloads the .cyacd file contents which is stored as string array
*
* Parameters:  
* bootloadImagePtr - Pointer to the string array
* lineCount - No. of lines in the .cyacd file(No: of rows in the string array)
*
* Return: 
*  Returns a flag to indicate whether the bootload operation was successful or not
*
*
****************************************************************************************************/
unsigned char BootloadStringImage(const char *bootloadImagePtr[],unsigned int lineCount )
{
	unsigned char err;
	unsigned char arrayId; 
	unsigned short rowNum;
	unsigned short rowSize; 
	unsigned char checksum ;
	unsigned char checksum2;
	unsigned long blVer=0;
	/* rowData buffer size should be equal to the length of data to be send for each flash row 
	* Equals 128
	*/
	unsigned char rowData[128];
	unsigned int lineLen;
	unsigned long  siliconID;
	unsigned char siliconRev;
	unsigned char packetChkSumType;
	unsigned int lineCntr ;
	
	/* Initialize line counter */
	lineCntr = 0u;
	
	/* Get length of the first line in cyacd file*/
	lineLen = strlen(bootloadImagePtr[lineCntr]);

	/* Parse the first line(header) of cyacd file to extract siliconID, siliconRev and packetChkSumType */
	err = CyBtldr_ParseHeader(lineLen ,(unsigned char *)bootloadImagePtr[lineCntr] , &siliconID , &siliconRev ,&packetChkSumType);
    
	/* Set the packet checksum type for communicating with bootloader. The packet checksum type to be used 
	* is determined from the cyacd file header information */
	CyBtldr_SetCheckSumType((CyBtldr_ChecksumType)packetChkSumType);
	
	if(err == CYRET_SUCCESS)
	{
		/* Start Bootloader operation */
		err = CyBtldr_StartBootloadOperation(&comm1 ,siliconID, siliconRev ,&blVer);
		lineCntr++ ;
		while((err == CYRET_SUCCESS)&& ( lineCntr <  lineCount ))
		{
            /* Get the string length for the line*/
			lineLen =  strlen(bootloadImagePtr[lineCntr]);
			/*Parse row data*/
			err = CyBtldr_ParseRowData((unsigned int)lineLen,(unsigned char *)bootloadImagePtr[lineCntr], &arrayId, &rowNum, rowData, &rowSize, &checksum);
          
			if (CYRET_SUCCESS == err)
            {
				/* Program Row */
				err = CyBtldr_ProgramRow(arrayId, rowNum, rowData, rowSize);
	            if (CYRET_SUCCESS == err)
				{
					/* Verify Row . Check whether the checksum received from bootloader matches
					* the expected row checksum stored in cyacd file*/
					checksum2 = (unsigned char)(checksum + arrayId + rowNum + (rowNum >> 8) + rowSize + (rowSize >> 8));
					err = CyBtldr_VerifyRow(arrayId, rowNum, checksum2);
				}
            }
			/* Increment the linCntr */
			lineCntr ++;
		}
		/* End Bootloader Operation */
		CyBtldr_EndBootloadOperation();
	}
	return(err);
}


/* [] END OF FILE */
