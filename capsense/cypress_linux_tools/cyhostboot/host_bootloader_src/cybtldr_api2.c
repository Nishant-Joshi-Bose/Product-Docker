/*******************************************************************************
* Copyright 2011-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include <string.h>
#include "cybtldr_parse.h"
#include "cybtldr_command.h"
#include "cybtldr_api.h"
#include "cybtldr_api2.h"

unsigned char g_abort;

int CyBtldr_RunAction(CyBtldr_Action action, const char* file, const unsigned char* securityKey,
    unsigned char appId, CyBtldr_CommunicationsData* comm, CyBtldr_ProgressUpdate* update)
{
    const unsigned long BL_VER_SUPPORT_VERIFY = 0x010214; /* Support for full flash verify added in v2.20 of cy_boot */
    const unsigned char INVALID_APP = 0xFF;

    unsigned long blVer = 0;
    unsigned long siliconId = 0;
    unsigned short rowNum = 0;
    unsigned short bufSize = 0;
    unsigned char siliconRev = 0;
    unsigned char chksumtype = SUM_CHECKSUM;
    unsigned char checksum = 0;
    unsigned char checksum2 = 0;
    unsigned char arrayId = 0;
    unsigned char isValid;
    unsigned char isActive;
    unsigned char buffer[MAX_BUFFER_SIZE];
    char line[MAX_BUFFER_SIZE];
    unsigned int lineLen;
    int err;
    unsigned char bootloaderEntered = 0;

    g_abort = 0;

    err = CyBtldr_OpenDataFile(file);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
    if (CYRET_SUCCESS == err)
    {
        err = CyBtldr_ReadLine(&lineLen, line);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
        if (CYRET_SUCCESS == err)
            err = CyBtldr_ParseHeader(lineLen, line, &siliconId, &siliconRev, &chksumtype);

printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
        if (CYRET_SUCCESS == err)
        {
            CyBtldr_SetCheckSumType(chksumtype);
            err = CyBtldr_StartBootloadOperation(comm, siliconId, siliconRev, &blVer, securityKey);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
            bootloaderEntered = 1;
        }

        appId -= 1; /* 1 and 2 are legal inputs to function. 0 and 1 are valid for bootloader component */
        if (appId > 1)
        {
            appId = INVALID_APP;
        }

        if ((CYRET_SUCCESS == err) && (appId != INVALID_APP))
        {
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
			/* This will return error if bootloader is for single app */
            err = CyBtldr_GetApplicationStatus(appId, &isValid, &isActive);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);

            /* Active app can be verified, but not programmed or erased */
            if (CYRET_SUCCESS == err && VERIFY != action && isActive)
			{
				/* This is multi app */
				err = CYRET_ERR_ACTIVE;
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
			}
			else if (CYBTLDR_STAT_ERR_CMD == (err ^ (int)CYRET_ERR_BTLDR_MASK))
			{
				/* Single app - restore previous CYRET_SUCCESS */
				err = CYRET_SUCCESS;
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
			}
        }

        if (CYRET_SUCCESS == err)
        {
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
            while (CYRET_SUCCESS == err)
            {
                if (g_abort)
                {
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                    err = CYRET_ABORT;
                    break;
                }

                err = CyBtldr_ReadLine(&lineLen, line);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                if (CYRET_SUCCESS == err)
                    err = CyBtldr_ParseRowData(lineLen, line, &arrayId, &rowNum, buffer, &bufSize, &checksum);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                if (CYRET_SUCCESS == err)
                {
                    switch (action)
                    {
                        case ERASE:
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                            err = CyBtldr_EraseRow(arrayId, rowNum);
                            break;
                        case PROGRAM:
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                            err = CyBtldr_ProgramRow(arrayId, rowNum, buffer, bufSize);
                            if (CYRET_SUCCESS != err)
                                break;
                            /* Continue on to verify the row that was programmed */
                        case VERIFY:
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                            checksum2 = (unsigned char)(checksum + arrayId + rowNum + (rowNum >> 8) + bufSize + (bufSize >> 8));
                            err = CyBtldr_VerifyRow(arrayId, rowNum, checksum2);
                            break;
                    }
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                    if (CYRET_SUCCESS == err && NULL != update)
                        update(arrayId, rowNum);
                }
                else if (CYRET_ERR_EOF == err)
                {
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
                    err = CYRET_SUCCESS;
                    break;
                }
            }

            if (CYRET_SUCCESS == err)
            {
                /* Set the active application to what was just programmed */
                if ((PROGRAM == action) && (INVALID_APP != appId))
                {
                    err = CyBtldr_GetApplicationStatus(appId, &isValid, &isActive);
printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);

                    if (CYRET_SUCCESS == err)
                    {
                        /* If valid set the active application to what was just programmed */
						/* This is multi app */
                        err = (0 == isValid)
                            ? CyBtldr_SetApplicationStatus(appId)
                            : CYRET_ERR_CHECKSUM;
                    }
					else if (CYBTLDR_STAT_ERR_CMD == (err ^ (int)CYRET_ERR_BTLDR_MASK))
					{
						/* Single app - restore previous CYRET_SUCCESS */
						err = CYRET_SUCCESS;
					}
                }

                /* Verify that the entire application is valid */
                else if ((PROGRAM == action || VERIFY == action) && (blVer >= BL_VER_SUPPORT_VERIFY))
                    err = CyBtldr_VerifyApplication();
            }

            CyBtldr_EndBootloadOperation();
        }
        else if (CYRET_ERR_COMM_MASK != (CYRET_ERR_COMM_MASK & err) && bootloaderEntered)
            CyBtldr_EndBootloadOperation();

        CyBtldr_CloseDataFile();
    }

printf("%s:%d, err: %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

int CyBtldr_Program(const char* file, const unsigned char* securityKey, unsigned char appId,
    CyBtldr_CommunicationsData* comm, CyBtldr_ProgressUpdate* update)
{
    return CyBtldr_RunAction(PROGRAM, file, securityKey, appId, comm, update);
}

int CyBtldr_Erase(const char* file, const unsigned char* securityKey, CyBtldr_CommunicationsData* comm,
    CyBtldr_ProgressUpdate* update)
{
    return CyBtldr_RunAction(ERASE, file, securityKey, 0, comm, update);
}

int CyBtldr_Verify(const char* file, const unsigned char* securityKey, CyBtldr_CommunicationsData* comm,
    CyBtldr_ProgressUpdate* update)
{
    return CyBtldr_RunAction(VERIFY, file, securityKey, 0, comm, update);
}

int CyBtldr_Abort(void)
{
    g_abort = 1;
    return CYRET_SUCCESS;
}
