/***************************************************************************//**
* \file CapSense_RegisterMap.h
* \version 3.10
*
* \brief
*   This file provides the definitions for the component data structure register.
*
* \see CapSense P4 v3.10 Datasheet
*
*//*****************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(CY_CAPSENSE_CapSense_REGISTER_MAP_H)
#define CY_CAPSENSE_CapSense_REGISTER_MAP_H

#include <cytypes.h>
#include "CapSense_Configuration.h"
#include "CapSense_Structure.h"

/*****************************************************************************/
/* RAM Data structure register definitions                                   */
/*****************************************************************************/
#define CapSense_CONFIG_ID_VALUE                            (CapSense_dsRam.configId)
#define CapSense_CONFIG_ID_OFFSET                           (0u)
#define CapSense_CONFIG_ID_SIZE                             (2u)
#define CapSense_CONFIG_ID_PARAM_ID                         (0x87000000u)

#define CapSense_DEVICE_ID_VALUE                            (CapSense_dsRam.deviceId)
#define CapSense_DEVICE_ID_OFFSET                           (2u)
#define CapSense_DEVICE_ID_SIZE                             (2u)
#define CapSense_DEVICE_ID_PARAM_ID                         (0x8B000002u)

#define CapSense_TUNER_CMD_VALUE                            (CapSense_dsRam.tunerCmd)
#define CapSense_TUNER_CMD_OFFSET                           (4u)
#define CapSense_TUNER_CMD_SIZE                             (2u)
#define CapSense_TUNER_CMD_PARAM_ID                         (0xAD000004u)

#define CapSense_SCAN_COUNTER_VALUE                         (CapSense_dsRam.scanCounter)
#define CapSense_SCAN_COUNTER_OFFSET                        (6u)
#define CapSense_SCAN_COUNTER_SIZE                          (2u)
#define CapSense_SCAN_COUNTER_PARAM_ID                      (0x8A000006u)

#define CapSense_STATUS_VALUE                               (CapSense_dsRam.status)
#define CapSense_STATUS_OFFSET                              (8u)
#define CapSense_STATUS_SIZE                                (4u)
#define CapSense_STATUS_PARAM_ID                            (0xCA000008u)

#define CapSense_WDGT_ENABLE0_VALUE                         (CapSense_dsRam.wdgtEnable[0u])
#define CapSense_WDGT_ENABLE0_OFFSET                        (12u)
#define CapSense_WDGT_ENABLE0_SIZE                          (4u)
#define CapSense_WDGT_ENABLE0_PARAM_ID                      (0xE000000Cu)

#define CapSense_WDGT_STATUS0_VALUE                         (CapSense_dsRam.wdgtStatus[0u])
#define CapSense_WDGT_STATUS0_OFFSET                        (16u)
#define CapSense_WDGT_STATUS0_SIZE                          (4u)
#define CapSense_WDGT_STATUS0_PARAM_ID                      (0xCD000010u)

#define CapSense_SNS_STATUS0_VALUE                          (CapSense_dsRam.snsStatus[0u])
#define CapSense_SNS_STATUS0_OFFSET                         (20u)
#define CapSense_SNS_STATUS0_SIZE                           (2u)
#define CapSense_SNS_STATUS0_PARAM_ID                       (0x83000014u)

#define CapSense_CSD0_CONFIG_VALUE                          (CapSense_dsRam.csd0Config)
#define CapSense_CSD0_CONFIG_OFFSET                         (22u)
#define CapSense_CSD0_CONFIG_SIZE                           (2u)
#define CapSense_CSD0_CONFIG_PARAM_ID                       (0xA9800016u)

#define CapSense_MOD_CSD_CLK_VALUE                          (CapSense_dsRam.modCsdClk)
#define CapSense_MOD_CSD_CLK_OFFSET                         (24u)
#define CapSense_MOD_CSD_CLK_SIZE                           (1u)
#define CapSense_MOD_CSD_CLK_PARAM_ID                       (0x6E800018u)

#define CapSense_LINEARSLIDER0_RESOLUTION_VALUE             (CapSense_dsRam.wdgtList.linearslider0.resolution)
#define CapSense_LINEARSLIDER0_RESOLUTION_OFFSET            (26u)
#define CapSense_LINEARSLIDER0_RESOLUTION_SIZE              (2u)
#define CapSense_LINEARSLIDER0_RESOLUTION_PARAM_ID          (0x8180001Au)

#define CapSense_LINEARSLIDER0_FINGER_TH_VALUE              (CapSense_dsRam.wdgtList.linearslider0.fingerTh)
#define CapSense_LINEARSLIDER0_FINGER_TH_OFFSET             (28u)
#define CapSense_LINEARSLIDER0_FINGER_TH_SIZE               (2u)
#define CapSense_LINEARSLIDER0_FINGER_TH_PARAM_ID           (0x8C80001Cu)

#define CapSense_LINEARSLIDER0_NOISE_TH_VALUE               (CapSense_dsRam.wdgtList.linearslider0.noiseTh)
#define CapSense_LINEARSLIDER0_NOISE_TH_OFFSET              (30u)
#define CapSense_LINEARSLIDER0_NOISE_TH_SIZE                (1u)
#define CapSense_LINEARSLIDER0_NOISE_TH_PARAM_ID            (0x4880001Eu)

#define CapSense_LINEARSLIDER0_NNOISE_TH_VALUE              (CapSense_dsRam.wdgtList.linearslider0.nNoiseTh)
#define CapSense_LINEARSLIDER0_NNOISE_TH_OFFSET             (31u)
#define CapSense_LINEARSLIDER0_NNOISE_TH_SIZE               (1u)
#define CapSense_LINEARSLIDER0_NNOISE_TH_PARAM_ID           (0x4E80001Fu)

#define CapSense_LINEARSLIDER0_HYSTERESIS_VALUE             (CapSense_dsRam.wdgtList.linearslider0.hysteresis)
#define CapSense_LINEARSLIDER0_HYSTERESIS_OFFSET            (32u)
#define CapSense_LINEARSLIDER0_HYSTERESIS_SIZE              (1u)
#define CapSense_LINEARSLIDER0_HYSTERESIS_PARAM_ID          (0x48800020u)

#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_VALUE            (CapSense_dsRam.wdgtList.linearslider0.onDebounce)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_OFFSET           (33u)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_SIZE             (1u)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_PARAM_ID         (0x4E800021u)

#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_VALUE           (CapSense_dsRam.wdgtList.linearslider0.lowBslnRst)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_OFFSET          (34u)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_SIZE            (1u)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_PARAM_ID        (0x44800022u)

#define CapSense_LINEARSLIDER0_BSLN_COEFF_VALUE             (CapSense_dsRam.wdgtList.linearslider0.bslnCoeff)
#define CapSense_LINEARSLIDER0_BSLN_COEFF_OFFSET            (35u)
#define CapSense_LINEARSLIDER0_BSLN_COEFF_SIZE              (1u)
#define CapSense_LINEARSLIDER0_BSLN_COEFF_PARAM_ID          (0x69800023u)

#define CapSense_LINEARSLIDER0_IDAC_MOD0_VALUE              (CapSense_dsRam.wdgtList.linearslider0.idacMod[0u])
#define CapSense_LINEARSLIDER0_IDAC_MOD0_OFFSET             (36u)
#define CapSense_LINEARSLIDER0_IDAC_MOD0_SIZE               (1u)
#define CapSense_LINEARSLIDER0_IDAC_MOD0_PARAM_ID           (0x44000024u)

#define CapSense_LINEARSLIDER0_SNS_CLK_VALUE                (CapSense_dsRam.wdgtList.linearslider0.snsClk)
#define CapSense_LINEARSLIDER0_SNS_CLK_OFFSET               (37u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SIZE                 (1u)
#define CapSense_LINEARSLIDER0_SNS_CLK_PARAM_ID             (0x4F800025u)

#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_VALUE         (CapSense_dsRam.wdgtList.linearslider0.snsClkSource)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_OFFSET        (38u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_PARAM_ID      (0x45800026u)

#define CapSense_LINEARSLIDER0_FINGER_CAP_VALUE             (CapSense_dsRam.wdgtList.linearslider0.fingerCap)
#define CapSense_LINEARSLIDER0_FINGER_CAP_OFFSET            (40u)
#define CapSense_LINEARSLIDER0_FINGER_CAP_SIZE              (2u)
#define CapSense_LINEARSLIDER0_FINGER_CAP_PARAM_ID          (0xA4000028u)

#define CapSense_LINEARSLIDER0_SIGPFC_VALUE                 (CapSense_dsRam.wdgtList.linearslider0.sigPFC)
#define CapSense_LINEARSLIDER0_SIGPFC_OFFSET                (42u)
#define CapSense_LINEARSLIDER0_SIGPFC_SIZE                  (2u)
#define CapSense_LINEARSLIDER0_SIGPFC_PARAM_ID              (0xA800002Au)

#define CapSense_LINEARSLIDER0_POSITION0_VALUE              (CapSense_dsRam.wdgtList.linearslider0.position[0u])
#define CapSense_LINEARSLIDER0_POSITION0_OFFSET             (44u)
#define CapSense_LINEARSLIDER0_POSITION0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_POSITION0_PARAM_ID           (0x8E00002Cu)

#define CapSense_LINEARSLIDER0_SNS0_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[0u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS0_RAW0_OFFSET             (46u)
#define CapSense_LINEARSLIDER0_SNS0_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS0_RAW0_PARAM_ID           (0x8200002Eu)

#define CapSense_LINEARSLIDER0_SNS0_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[0u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS0_BSLN0_OFFSET            (48u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN0_PARAM_ID          (0x88000030u)

#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[0u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_OFFSET        (50u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_PARAM_ID      (0x4C000032u)

#define CapSense_LINEARSLIDER0_SNS0_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[0u].diff)
#define CapSense_LINEARSLIDER0_SNS0_DIFF_OFFSET             (52u)
#define CapSense_LINEARSLIDER0_SNS0_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS0_DIFF_PARAM_ID           (0x89000034u)

#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[0u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_OFFSET (54u)
#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID (0x4D000036u)

#define CapSense_LINEARSLIDER0_SNS0_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[0u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS0_POS_BLSN_RST_CNT0_OFFSET (56u)
#define CapSense_LINEARSLIDER0_SNS0_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS0_POS_BLSN_RST_CNT0_PARAM_ID (0x8A000038u)

#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[0u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_OFFSET       (58u)
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_PARAM_ID     (0x4E00003Au)

#define CapSense_LINEARSLIDER0_SNS1_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[1u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS1_RAW0_OFFSET             (60u)
#define CapSense_LINEARSLIDER0_SNS1_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS1_RAW0_PARAM_ID           (0x8B00003Cu)

#define CapSense_LINEARSLIDER0_SNS1_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[1u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS1_BSLN0_OFFSET            (62u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN0_PARAM_ID          (0x8700003Eu)

#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[1u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_OFFSET        (64u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_PARAM_ID      (0x42000040u)

#define CapSense_LINEARSLIDER0_SNS1_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[1u].diff)
#define CapSense_LINEARSLIDER0_SNS1_DIFF_OFFSET             (66u)
#define CapSense_LINEARSLIDER0_SNS1_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS1_DIFF_PARAM_ID           (0x86000042u)

#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[1u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_OFFSET (68u)
#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_PARAM_ID (0x43000044u)

#define CapSense_LINEARSLIDER0_SNS1_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[1u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS1_POS_BLSN_RST_CNT0_OFFSET (70u)
#define CapSense_LINEARSLIDER0_SNS1_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS1_POS_BLSN_RST_CNT0_PARAM_ID (0x87000046u)

#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[1u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_OFFSET       (72u)
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_PARAM_ID     (0x40000048u)

#define CapSense_LINEARSLIDER0_SNS2_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[2u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS2_RAW0_OFFSET             (74u)
#define CapSense_LINEARSLIDER0_SNS2_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS2_RAW0_PARAM_ID           (0x8400004Au)

#define CapSense_LINEARSLIDER0_SNS2_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[2u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS2_BSLN0_OFFSET            (76u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN0_PARAM_ID          (0x8900004Cu)

#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[2u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_OFFSET        (78u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_PARAM_ID      (0x4D00004Eu)

#define CapSense_LINEARSLIDER0_SNS2_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[2u].diff)
#define CapSense_LINEARSLIDER0_SNS2_DIFF_OFFSET             (80u)
#define CapSense_LINEARSLIDER0_SNS2_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS2_DIFF_PARAM_ID           (0x8F000050u)

#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[2u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_OFFSET (82u)
#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_PARAM_ID (0x4B000052u)

#define CapSense_LINEARSLIDER0_SNS2_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[2u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS2_POS_BLSN_RST_CNT0_OFFSET (84u)
#define CapSense_LINEARSLIDER0_SNS2_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS2_POS_BLSN_RST_CNT0_PARAM_ID (0x8E000054u)

#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[2u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_OFFSET       (86u)
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_PARAM_ID     (0x4A000056u)

#define CapSense_LINEARSLIDER0_SNS3_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[3u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS3_RAW0_OFFSET             (88u)
#define CapSense_LINEARSLIDER0_SNS3_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS3_RAW0_PARAM_ID           (0x8D000058u)

#define CapSense_LINEARSLIDER0_SNS3_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[3u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS3_BSLN0_OFFSET            (90u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN0_PARAM_ID          (0x8100005Au)

#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[3u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_OFFSET        (92u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_PARAM_ID      (0x4400005Cu)

#define CapSense_LINEARSLIDER0_SNS3_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[3u].diff)
#define CapSense_LINEARSLIDER0_SNS3_DIFF_OFFSET             (94u)
#define CapSense_LINEARSLIDER0_SNS3_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS3_DIFF_PARAM_ID           (0x8000005Eu)

#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[3u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_OFFSET (96u)
#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_PARAM_ID (0x48000060u)

#define CapSense_LINEARSLIDER0_SNS3_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[3u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS3_POS_BLSN_RST_CNT0_OFFSET (98u)
#define CapSense_LINEARSLIDER0_SNS3_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS3_POS_BLSN_RST_CNT0_PARAM_ID (0x8C000062u)

#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[3u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_OFFSET       (100u)
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_PARAM_ID     (0x49000064u)

#define CapSense_LINEARSLIDER0_SNS4_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[4u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS4_RAW0_OFFSET             (102u)
#define CapSense_LINEARSLIDER0_SNS4_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS4_RAW0_PARAM_ID           (0x8D000066u)

#define CapSense_LINEARSLIDER0_SNS4_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[4u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS4_BSLN0_OFFSET            (104u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN0_PARAM_ID          (0x82000068u)

#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[4u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_OFFSET        (106u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_PARAM_ID      (0x4600006Au)

#define CapSense_LINEARSLIDER0_SNS4_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[4u].diff)
#define CapSense_LINEARSLIDER0_SNS4_DIFF_OFFSET             (108u)
#define CapSense_LINEARSLIDER0_SNS4_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS4_DIFF_PARAM_ID           (0x8300006Cu)

#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[4u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_OFFSET (110u)
#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_PARAM_ID (0x4700006Eu)

#define CapSense_LINEARSLIDER0_SNS4_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[4u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS4_POS_BLSN_RST_CNT0_OFFSET (112u)
#define CapSense_LINEARSLIDER0_SNS4_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS4_POS_BLSN_RST_CNT0_PARAM_ID (0x85000070u)

#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[4u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_OFFSET       (114u)
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_PARAM_ID     (0x41000072u)

#define CapSense_LINEARSLIDER0_SNS5_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[5u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS5_RAW0_OFFSET             (116u)
#define CapSense_LINEARSLIDER0_SNS5_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS5_RAW0_PARAM_ID           (0x84000074u)

#define CapSense_LINEARSLIDER0_SNS5_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[5u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS5_BSLN0_OFFSET            (118u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN0_PARAM_ID          (0x88000076u)

#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[5u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_OFFSET        (120u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_PARAM_ID      (0x4F000078u)

#define CapSense_LINEARSLIDER0_SNS5_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[5u].diff)
#define CapSense_LINEARSLIDER0_SNS5_DIFF_OFFSET             (122u)
#define CapSense_LINEARSLIDER0_SNS5_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS5_DIFF_PARAM_ID           (0x8B00007Au)

#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[5u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_OFFSET (124u)
#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_PARAM_ID (0x4E00007Cu)

#define CapSense_LINEARSLIDER0_SNS5_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[5u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS5_POS_BLSN_RST_CNT0_OFFSET (126u)
#define CapSense_LINEARSLIDER0_SNS5_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS5_POS_BLSN_RST_CNT0_PARAM_ID (0x8A00007Eu)

#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[5u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_OFFSET       (128u)
#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_PARAM_ID     (0x4C000080u)

#define CapSense_LINEARSLIDER0_SNS6_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[6u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS6_RAW0_OFFSET             (130u)
#define CapSense_LINEARSLIDER0_SNS6_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS6_RAW0_PARAM_ID           (0x88000082u)

#define CapSense_LINEARSLIDER0_SNS6_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[6u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS6_BSLN0_OFFSET            (132u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN0_PARAM_ID          (0x85000084u)

#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[6u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_OFFSET        (134u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_PARAM_ID      (0x41000086u)

#define CapSense_LINEARSLIDER0_SNS6_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[6u].diff)
#define CapSense_LINEARSLIDER0_SNS6_DIFF_OFFSET             (136u)
#define CapSense_LINEARSLIDER0_SNS6_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS6_DIFF_PARAM_ID           (0x86000088u)

#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[6u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_OFFSET (138u)
#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_PARAM_ID (0x4200008Au)

#define CapSense_LINEARSLIDER0_SNS6_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[6u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS6_POS_BLSN_RST_CNT0_OFFSET (140u)
#define CapSense_LINEARSLIDER0_SNS6_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS6_POS_BLSN_RST_CNT0_PARAM_ID (0x8700008Cu)

#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[6u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_OFFSET       (142u)
#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_PARAM_ID     (0x4300008Eu)

#define CapSense_LINEARSLIDER0_SNS7_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[7u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS7_RAW0_OFFSET             (144u)
#define CapSense_LINEARSLIDER0_SNS7_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS7_RAW0_PARAM_ID           (0x81000090u)

#define CapSense_LINEARSLIDER0_SNS7_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[7u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS7_BSLN0_OFFSET            (146u)
#define CapSense_LINEARSLIDER0_SNS7_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS7_BSLN0_PARAM_ID          (0x8D000092u)

#define CapSense_LINEARSLIDER0_SNS7_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[7u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS7_BSLN_EXT0_OFFSET        (148u)
#define CapSense_LINEARSLIDER0_SNS7_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS7_BSLN_EXT0_PARAM_ID      (0x48000094u)

#define CapSense_LINEARSLIDER0_SNS7_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[7u].diff)
#define CapSense_LINEARSLIDER0_SNS7_DIFF_OFFSET             (150u)
#define CapSense_LINEARSLIDER0_SNS7_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS7_DIFF_PARAM_ID           (0x8C000096u)

#define CapSense_LINEARSLIDER0_SNS7_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[7u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS7_NEG_BSLN_RST_CNT0_OFFSET (152u)
#define CapSense_LINEARSLIDER0_SNS7_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS7_NEG_BSLN_RST_CNT0_PARAM_ID (0x4B000098u)

#define CapSense_LINEARSLIDER0_SNS7_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[7u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS7_POS_BLSN_RST_CNT0_OFFSET (154u)
#define CapSense_LINEARSLIDER0_SNS7_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS7_POS_BLSN_RST_CNT0_PARAM_ID (0x8F00009Au)

#define CapSense_LINEARSLIDER0_SNS7_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[7u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS7_IDAC_COMP0_OFFSET       (156u)
#define CapSense_LINEARSLIDER0_SNS7_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS7_IDAC_COMP0_PARAM_ID     (0x4A00009Cu)

#define CapSense_LINEARSLIDER0_SNS8_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[8u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS8_RAW0_OFFSET             (158u)
#define CapSense_LINEARSLIDER0_SNS8_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS8_RAW0_PARAM_ID           (0x8E00009Eu)

#define CapSense_LINEARSLIDER0_SNS8_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[8u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS8_BSLN0_OFFSET            (160u)
#define CapSense_LINEARSLIDER0_SNS8_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS8_BSLN0_PARAM_ID          (0x8E0000A0u)

#define CapSense_LINEARSLIDER0_SNS8_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[8u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS8_BSLN_EXT0_OFFSET        (162u)
#define CapSense_LINEARSLIDER0_SNS8_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS8_BSLN_EXT0_PARAM_ID      (0x4A0000A2u)

#define CapSense_LINEARSLIDER0_SNS8_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[8u].diff)
#define CapSense_LINEARSLIDER0_SNS8_DIFF_OFFSET             (164u)
#define CapSense_LINEARSLIDER0_SNS8_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS8_DIFF_PARAM_ID           (0x8F0000A4u)

#define CapSense_LINEARSLIDER0_SNS8_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[8u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS8_NEG_BSLN_RST_CNT0_OFFSET (166u)
#define CapSense_LINEARSLIDER0_SNS8_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS8_NEG_BSLN_RST_CNT0_PARAM_ID (0x4B0000A6u)

#define CapSense_LINEARSLIDER0_SNS8_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[8u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS8_POS_BLSN_RST_CNT0_OFFSET (168u)
#define CapSense_LINEARSLIDER0_SNS8_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS8_POS_BLSN_RST_CNT0_PARAM_ID (0x8C0000A8u)

#define CapSense_LINEARSLIDER0_SNS8_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[8u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS8_IDAC_COMP0_OFFSET       (170u)
#define CapSense_LINEARSLIDER0_SNS8_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS8_IDAC_COMP0_PARAM_ID     (0x480000AAu)

#define CapSense_LINEARSLIDER0_SNS9_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[9u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS9_RAW0_OFFSET             (172u)
#define CapSense_LINEARSLIDER0_SNS9_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS9_RAW0_PARAM_ID           (0x8D0000ACu)

#define CapSense_LINEARSLIDER0_SNS9_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[9u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS9_BSLN0_OFFSET            (174u)
#define CapSense_LINEARSLIDER0_SNS9_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS9_BSLN0_PARAM_ID          (0x810000AEu)

#define CapSense_LINEARSLIDER0_SNS9_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[9u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS9_BSLN_EXT0_OFFSET        (176u)
#define CapSense_LINEARSLIDER0_SNS9_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS9_BSLN_EXT0_PARAM_ID      (0x430000B0u)

#define CapSense_LINEARSLIDER0_SNS9_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[9u].diff)
#define CapSense_LINEARSLIDER0_SNS9_DIFF_OFFSET             (178u)
#define CapSense_LINEARSLIDER0_SNS9_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS9_DIFF_PARAM_ID           (0x870000B2u)

#define CapSense_LINEARSLIDER0_SNS9_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[9u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS9_NEG_BSLN_RST_CNT0_OFFSET (180u)
#define CapSense_LINEARSLIDER0_SNS9_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS9_NEG_BSLN_RST_CNT0_PARAM_ID (0x420000B4u)

#define CapSense_LINEARSLIDER0_SNS9_POS_BLSN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[9u].posBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS9_POS_BLSN_RST_CNT0_OFFSET (182u)
#define CapSense_LINEARSLIDER0_SNS9_POS_BLSN_RST_CNT0_SIZE  (2u)
#define CapSense_LINEARSLIDER0_SNS9_POS_BLSN_RST_CNT0_PARAM_ID (0x860000B6u)

#define CapSense_LINEARSLIDER0_SNS9_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[9u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS9_IDAC_COMP0_OFFSET       (184u)
#define CapSense_LINEARSLIDER0_SNS9_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS9_IDAC_COMP0_PARAM_ID     (0x410000B8u)

#define CapSense_SNR_TEST_WIDGET_ID_VALUE                   (CapSense_dsRam.snrTestWidgetId)
#define CapSense_SNR_TEST_WIDGET_ID_OFFSET                  (186u)
#define CapSense_SNR_TEST_WIDGET_ID_SIZE                    (1u)
#define CapSense_SNR_TEST_WIDGET_ID_PARAM_ID                (0x660000BAu)

#define CapSense_SNR_TEST_SENSOR_ID_VALUE                   (CapSense_dsRam.snrTestSensorId)
#define CapSense_SNR_TEST_SENSOR_ID_OFFSET                  (187u)
#define CapSense_SNR_TEST_SENSOR_ID_SIZE                    (1u)
#define CapSense_SNR_TEST_SENSOR_ID_PARAM_ID                (0x600000BBu)

#define CapSense_SNR_TEST_SCAN_COUNTER_VALUE                (CapSense_dsRam.snrTestScanCounter)
#define CapSense_SNR_TEST_SCAN_COUNTER_OFFSET               (188u)
#define CapSense_SNR_TEST_SCAN_COUNTER_SIZE                 (2u)
#define CapSense_SNR_TEST_SCAN_COUNTER_PARAM_ID             (0x880000BCu)

#define CapSense_SNR_TEST_RAW_COUNT0_VALUE                  (CapSense_dsRam.snrTestRawCount[0u])
#define CapSense_SNR_TEST_RAW_COUNT0_OFFSET                 (190u)
#define CapSense_SNR_TEST_RAW_COUNT0_SIZE                   (2u)
#define CapSense_SNR_TEST_RAW_COUNT0_PARAM_ID               (0x840000BEu)


/*****************************************************************************/
/* Flash Data structure register definitions                                 */
/*****************************************************************************/
#define CapSense_LINEARSLIDER0_PTR2SNS_FLASH_VALUE          (CapSense_dsFlash.wdgtArray[0].ptr2SnsFlash)
#define CapSense_LINEARSLIDER0_PTR2SNS_FLASH_OFFSET         (0u)
#define CapSense_LINEARSLIDER0_PTR2SNS_FLASH_SIZE           (4u)
#define CapSense_LINEARSLIDER0_PTR2SNS_FLASH_PARAM_ID       (0xD1000000u)

#define CapSense_LINEARSLIDER0_PTR2WD_RAM_VALUE             (CapSense_dsFlash.wdgtArray[0].ptr2WdgtRam)
#define CapSense_LINEARSLIDER0_PTR2WD_RAM_OFFSET            (4u)
#define CapSense_LINEARSLIDER0_PTR2WD_RAM_SIZE              (4u)
#define CapSense_LINEARSLIDER0_PTR2WD_RAM_PARAM_ID          (0xD0000004u)

#define CapSense_LINEARSLIDER0_PTR2SNS_RAM_VALUE            (CapSense_dsFlash.wdgtArray[0].ptr2SnsRam)
#define CapSense_LINEARSLIDER0_PTR2SNS_RAM_OFFSET           (8u)
#define CapSense_LINEARSLIDER0_PTR2SNS_RAM_SIZE             (4u)
#define CapSense_LINEARSLIDER0_PTR2SNS_RAM_PARAM_ID         (0xD3000008u)

#define CapSense_LINEARSLIDER0_PTR2FLTR_HISTORY_VALUE       (CapSense_dsFlash.wdgtArray[0].ptr2FltrHistory)
#define CapSense_LINEARSLIDER0_PTR2FLTR_HISTORY_OFFSET      (12u)
#define CapSense_LINEARSLIDER0_PTR2FLTR_HISTORY_SIZE        (4u)
#define CapSense_LINEARSLIDER0_PTR2FLTR_HISTORY_PARAM_ID    (0xD200000Cu)

#define CapSense_LINEARSLIDER0_PTR2DEBOUNCE_VALUE           (CapSense_dsFlash.wdgtArray[0].ptr2DebounceArr)
#define CapSense_LINEARSLIDER0_PTR2DEBOUNCE_OFFSET          (16u)
#define CapSense_LINEARSLIDER0_PTR2DEBOUNCE_SIZE            (4u)
#define CapSense_LINEARSLIDER0_PTR2DEBOUNCE_PARAM_ID        (0xD4000010u)

#define CapSense_LINEARSLIDER0_STATIC_CONFIG_VALUE          (CapSense_dsFlash.wdgtArray[0].staticConfig)
#define CapSense_LINEARSLIDER0_STATIC_CONFIG_OFFSET         (20u)
#define CapSense_LINEARSLIDER0_STATIC_CONFIG_SIZE           (2u)
#define CapSense_LINEARSLIDER0_STATIC_CONFIG_PARAM_ID       (0x9A000014u)

#define CapSense_LINEARSLIDER0_TOTAL_NUM_SNS_VALUE          (CapSense_dsFlash.wdgtArray[0].totalNumSns)
#define CapSense_LINEARSLIDER0_TOTAL_NUM_SNS_OFFSET         (22u)
#define CapSense_LINEARSLIDER0_TOTAL_NUM_SNS_SIZE           (2u)
#define CapSense_LINEARSLIDER0_TOTAL_NUM_SNS_PARAM_ID       (0x96000016u)

#define CapSense_LINEARSLIDER0_TYPE_VALUE                   (CapSense_dsFlash.wdgtArray[0].wdgtType)
#define CapSense_LINEARSLIDER0_TYPE_OFFSET                  (24u)
#define CapSense_LINEARSLIDER0_TYPE_SIZE                    (1u)
#define CapSense_LINEARSLIDER0_TYPE_PARAM_ID                (0x51000018u)

#define CapSense_LINEARSLIDER0_NUM_COLS_VALUE               (CapSense_dsFlash.wdgtArray[0].numCols)
#define CapSense_LINEARSLIDER0_NUM_COLS_OFFSET              (25u)
#define CapSense_LINEARSLIDER0_NUM_COLS_SIZE                (1u)
#define CapSense_LINEARSLIDER0_NUM_COLS_PARAM_ID            (0x57000019u)

#define CapSense_LINEARSLIDER0_X_RESOLUTION_VALUE           (CapSense_dsFlash.wdgtArray[0].xResolution)
#define CapSense_LINEARSLIDER0_X_RESOLUTION_OFFSET          (26u)
#define CapSense_LINEARSLIDER0_X_RESOLUTION_SIZE            (2u)
#define CapSense_LINEARSLIDER0_X_RESOLUTION_PARAM_ID        (0x9500001Au)

#define CapSense_LINEARSLIDER0_X_CENT_MULT_VALUE            (CapSense_dsFlash.wdgtArray[0].xCentroidMultiplier)
#define CapSense_LINEARSLIDER0_X_CENT_MULT_OFFSET           (28u)
#define CapSense_LINEARSLIDER0_X_CENT_MULT_SIZE             (4u)
#define CapSense_LINEARSLIDER0_X_CENT_MULT_PARAM_ID         (0xD700001Cu)

#define CapSense_LINEARSLIDER0_PTR2NOISE_ENVLP_VALUE        (CapSense_dsFlash.wdgtArray[0].ptr2NoiseEnvlp)
#define CapSense_LINEARSLIDER0_PTR2NOISE_ENVLP_OFFSET       (32u)
#define CapSense_LINEARSLIDER0_PTR2NOISE_ENVLP_SIZE         (4u)
#define CapSense_LINEARSLIDER0_PTR2NOISE_ENVLP_PARAM_ID     (0xDB000020u)

#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_VALUE        (CapSense_dsFlash.wdgtArray[0].ptr2PosHistory)
#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_OFFSET       (36u)
#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_SIZE         (4u)
#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_PARAM_ID     (0xDA000024u)


#endif /* End CY_CAPSENSE_CapSense_REGISTER_MAP_H */

/* [] END OF FILE */
