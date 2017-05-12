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
#define CapSense_SNS_STATUS0_SIZE                           (1u)
#define CapSense_SNS_STATUS0_PARAM_ID                       (0x4B000014u)

#define CapSense_SNS_STATUS1_VALUE                          (CapSense_dsRam.snsStatus[1u])
#define CapSense_SNS_STATUS1_OFFSET                         (21u)
#define CapSense_SNS_STATUS1_SIZE                           (1u)
#define CapSense_SNS_STATUS1_PARAM_ID                       (0x4D000015u)

#define CapSense_SNS_STATUS2_VALUE                          (CapSense_dsRam.snsStatus[2u])
#define CapSense_SNS_STATUS2_OFFSET                         (22u)
#define CapSense_SNS_STATUS2_SIZE                           (1u)
#define CapSense_SNS_STATUS2_PARAM_ID                       (0x47000016u)

#define CapSense_SNS_STATUS3_VALUE                          (CapSense_dsRam.snsStatus[3u])
#define CapSense_SNS_STATUS3_OFFSET                         (23u)
#define CapSense_SNS_STATUS3_SIZE                           (1u)
#define CapSense_SNS_STATUS3_PARAM_ID                       (0x41000017u)

#define CapSense_SNS_STATUS4_VALUE                          (CapSense_dsRam.snsStatus[4u])
#define CapSense_SNS_STATUS4_OFFSET                         (24u)
#define CapSense_SNS_STATUS4_SIZE                           (1u)
#define CapSense_SNS_STATUS4_PARAM_ID                       (0x48000018u)

#define CapSense_SNS_STATUS5_VALUE                          (CapSense_dsRam.snsStatus[5u])
#define CapSense_SNS_STATUS5_OFFSET                         (25u)
#define CapSense_SNS_STATUS5_SIZE                           (1u)
#define CapSense_SNS_STATUS5_PARAM_ID                       (0x4E000019u)

#define CapSense_SNS_STATUS6_VALUE                          (CapSense_dsRam.snsStatus[6u])
#define CapSense_SNS_STATUS6_OFFSET                         (26u)
#define CapSense_SNS_STATUS6_SIZE                           (1u)
#define CapSense_SNS_STATUS6_PARAM_ID                       (0x4400001Au)

#define CapSense_SNS_STATUS7_VALUE                          (CapSense_dsRam.snsStatus[7u])
#define CapSense_SNS_STATUS7_OFFSET                         (27u)
#define CapSense_SNS_STATUS7_SIZE                           (1u)
#define CapSense_SNS_STATUS7_PARAM_ID                       (0x4200001Bu)

#define CapSense_CSD0_CONFIG_VALUE                          (CapSense_dsRam.csd0Config)
#define CapSense_CSD0_CONFIG_OFFSET                         (28u)
#define CapSense_CSD0_CONFIG_SIZE                           (2u)
#define CapSense_CSD0_CONFIG_PARAM_ID                       (0xA780001Cu)

#define CapSense_MOD_CSD_CLK_VALUE                          (CapSense_dsRam.modCsdClk)
#define CapSense_MOD_CSD_CLK_OFFSET                         (30u)
#define CapSense_MOD_CSD_CLK_SIZE                           (1u)
#define CapSense_MOD_CSD_CLK_PARAM_ID                       (0x6380001Eu)

#define CapSense_LINEARSLIDER0_RESOLUTION_VALUE             (CapSense_dsRam.wdgtList.linearslider0.resolution)
#define CapSense_LINEARSLIDER0_RESOLUTION_OFFSET            (32u)
#define CapSense_LINEARSLIDER0_RESOLUTION_SIZE              (2u)
#define CapSense_LINEARSLIDER0_RESOLUTION_PARAM_ID          (0x80800020u)

#define CapSense_LINEARSLIDER0_FINGER_TH_VALUE              (CapSense_dsRam.wdgtList.linearslider0.fingerTh)
#define CapSense_LINEARSLIDER0_FINGER_TH_OFFSET             (34u)
#define CapSense_LINEARSLIDER0_FINGER_TH_SIZE               (2u)
#define CapSense_LINEARSLIDER0_FINGER_TH_PARAM_ID           (0xA7800022u)

#define CapSense_LINEARSLIDER0_NOISE_TH_VALUE               (CapSense_dsRam.wdgtList.linearslider0.noiseTh)
#define CapSense_LINEARSLIDER0_NOISE_TH_OFFSET              (36u)
#define CapSense_LINEARSLIDER0_NOISE_TH_SIZE                (1u)
#define CapSense_LINEARSLIDER0_NOISE_TH_PARAM_ID            (0x62800024u)

#define CapSense_LINEARSLIDER0_NNOISE_TH_VALUE              (CapSense_dsRam.wdgtList.linearslider0.nNoiseTh)
#define CapSense_LINEARSLIDER0_NNOISE_TH_OFFSET             (37u)
#define CapSense_LINEARSLIDER0_NNOISE_TH_SIZE               (1u)
#define CapSense_LINEARSLIDER0_NNOISE_TH_PARAM_ID           (0x64800025u)

#define CapSense_LINEARSLIDER0_HYSTERESIS_VALUE             (CapSense_dsRam.wdgtList.linearslider0.hysteresis)
#define CapSense_LINEARSLIDER0_HYSTERESIS_OFFSET            (38u)
#define CapSense_LINEARSLIDER0_HYSTERESIS_SIZE              (1u)
#define CapSense_LINEARSLIDER0_HYSTERESIS_PARAM_ID          (0x6E800026u)

#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_VALUE            (CapSense_dsRam.wdgtList.linearslider0.onDebounce)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_OFFSET           (39u)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_SIZE             (1u)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE_PARAM_ID         (0x68800027u)

#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_VALUE           (CapSense_dsRam.wdgtList.linearslider0.lowBslnRst)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_OFFSET          (40u)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_SIZE            (1u)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST_PARAM_ID        (0x61800028u)

#define CapSense_LINEARSLIDER0_BSLN_COEFF_VALUE             (CapSense_dsRam.wdgtList.linearslider0.bslnCoeff)
#define CapSense_LINEARSLIDER0_BSLN_COEFF_OFFSET            (41u)
#define CapSense_LINEARSLIDER0_BSLN_COEFF_SIZE              (1u)
#define CapSense_LINEARSLIDER0_BSLN_COEFF_PARAM_ID          (0x67800029u)

#define CapSense_LINEARSLIDER0_IDAC_MOD0_VALUE              (CapSense_dsRam.wdgtList.linearslider0.idacMod[0u])
#define CapSense_LINEARSLIDER0_IDAC_MOD0_OFFSET             (42u)
#define CapSense_LINEARSLIDER0_IDAC_MOD0_SIZE               (1u)
#define CapSense_LINEARSLIDER0_IDAC_MOD0_PARAM_ID           (0x4B00002Au)

#define CapSense_LINEARSLIDER0_SNS_CLK_VALUE                (CapSense_dsRam.wdgtList.linearslider0.snsClk)
#define CapSense_LINEARSLIDER0_SNS_CLK_OFFSET               (43u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SIZE                 (1u)
#define CapSense_LINEARSLIDER0_SNS_CLK_PARAM_ID             (0x4080002Bu)

#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_VALUE         (CapSense_dsRam.wdgtList.linearslider0.snsClkSource)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_OFFSET        (44u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE_PARAM_ID      (0x4B80002Cu)

#define CapSense_LINEARSLIDER0_FINGER_CAP_VALUE             (CapSense_dsRam.wdgtList.linearslider0.fingerCap)
#define CapSense_LINEARSLIDER0_FINGER_CAP_OFFSET            (46u)
#define CapSense_LINEARSLIDER0_FINGER_CAP_SIZE              (2u)
#define CapSense_LINEARSLIDER0_FINGER_CAP_PARAM_ID          (0xA900002Eu)

#define CapSense_LINEARSLIDER0_SIGPFC_VALUE                 (CapSense_dsRam.wdgtList.linearslider0.sigPFC)
#define CapSense_LINEARSLIDER0_SIGPFC_OFFSET                (48u)
#define CapSense_LINEARSLIDER0_SIGPFC_SIZE                  (2u)
#define CapSense_LINEARSLIDER0_SIGPFC_PARAM_ID              (0xA3000030u)

#define CapSense_LINEARSLIDER0_POSITION0_VALUE              (CapSense_dsRam.wdgtList.linearslider0.position[0u])
#define CapSense_LINEARSLIDER0_POSITION0_OFFSET             (50u)
#define CapSense_LINEARSLIDER0_POSITION0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_POSITION0_PARAM_ID           (0x84000032u)

#define CapSense_BUT0_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but0.resolution)
#define CapSense_BUT0_RESOLUTION_OFFSET                     (52u)
#define CapSense_BUT0_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT0_RESOLUTION_PARAM_ID                   (0x87810034u)

#define CapSense_BUT0_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but0.fingerTh)
#define CapSense_BUT0_FINGER_TH_OFFSET                      (54u)
#define CapSense_BUT0_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT0_FINGER_TH_PARAM_ID                    (0xA0810036u)

#define CapSense_BUT0_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but0.noiseTh)
#define CapSense_BUT0_NOISE_TH_OFFSET                       (56u)
#define CapSense_BUT0_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT0_NOISE_TH_PARAM_ID                     (0x67810038u)

#define CapSense_BUT0_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but0.nNoiseTh)
#define CapSense_BUT0_NNOISE_TH_OFFSET                      (57u)
#define CapSense_BUT0_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT0_NNOISE_TH_PARAM_ID                    (0x61810039u)

#define CapSense_BUT0_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but0.hysteresis)
#define CapSense_BUT0_HYSTERESIS_OFFSET                     (58u)
#define CapSense_BUT0_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT0_HYSTERESIS_PARAM_ID                   (0x6B81003Au)

#define CapSense_BUT0_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but0.onDebounce)
#define CapSense_BUT0_ON_DEBOUNCE_OFFSET                    (59u)
#define CapSense_BUT0_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT0_ON_DEBOUNCE_PARAM_ID                  (0x6D81003Bu)

#define CapSense_BUT0_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but0.lowBslnRst)
#define CapSense_BUT0_LOW_BSLN_RST_OFFSET                   (60u)
#define CapSense_BUT0_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT0_LOW_BSLN_RST_PARAM_ID                 (0x6681003Cu)

#define CapSense_BUT0_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but0.bslnCoeff)
#define CapSense_BUT0_BSLN_COEFF_OFFSET                     (61u)
#define CapSense_BUT0_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT0_BSLN_COEFF_PARAM_ID                   (0x6081003Du)

#define CapSense_BUT0_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but0.idacMod[0u])
#define CapSense_BUT0_IDAC_MOD0_OFFSET                      (62u)
#define CapSense_BUT0_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT0_IDAC_MOD0_PARAM_ID                    (0x4C01003Eu)

#define CapSense_BUT0_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but0.snsClk)
#define CapSense_BUT0_SNS_CLK_OFFSET                        (63u)
#define CapSense_BUT0_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT0_SNS_CLK_PARAM_ID                      (0x4781003Fu)

#define CapSense_BUT0_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but0.snsClkSource)
#define CapSense_BUT0_SNS_CLK_SOURCE_OFFSET                 (64u)
#define CapSense_BUT0_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT0_SNS_CLK_SOURCE_PARAM_ID               (0x4C810040u)

#define CapSense_BUT0_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but0.fingerCap)
#define CapSense_BUT0_FINGER_CAP_OFFSET                     (66u)
#define CapSense_BUT0_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT0_FINGER_CAP_PARAM_ID                   (0xAE010042u)

#define CapSense_BUT0_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but0.sigPFC)
#define CapSense_BUT0_SIGPFC_OFFSET                         (68u)
#define CapSense_BUT0_SIGPFC_SIZE                           (2u)
#define CapSense_BUT0_SIGPFC_PARAM_ID                       (0xA3010044u)

#define CapSense_BUT1_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but1.resolution)
#define CapSense_BUT1_RESOLUTION_OFFSET                     (70u)
#define CapSense_BUT1_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT1_RESOLUTION_PARAM_ID                   (0x8C820046u)

#define CapSense_BUT1_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but1.fingerTh)
#define CapSense_BUT1_FINGER_TH_OFFSET                      (72u)
#define CapSense_BUT1_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT1_FINGER_TH_PARAM_ID                    (0xA8820048u)

#define CapSense_BUT1_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but1.noiseTh)
#define CapSense_BUT1_NOISE_TH_OFFSET                       (74u)
#define CapSense_BUT1_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT1_NOISE_TH_PARAM_ID                     (0x6C82004Au)

#define CapSense_BUT1_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but1.nNoiseTh)
#define CapSense_BUT1_NNOISE_TH_OFFSET                      (75u)
#define CapSense_BUT1_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT1_NNOISE_TH_PARAM_ID                    (0x6A82004Bu)

#define CapSense_BUT1_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but1.hysteresis)
#define CapSense_BUT1_HYSTERESIS_OFFSET                     (76u)
#define CapSense_BUT1_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT1_HYSTERESIS_PARAM_ID                   (0x6182004Cu)

#define CapSense_BUT1_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but1.onDebounce)
#define CapSense_BUT1_ON_DEBOUNCE_OFFSET                    (77u)
#define CapSense_BUT1_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT1_ON_DEBOUNCE_PARAM_ID                  (0x6782004Du)

#define CapSense_BUT1_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but1.lowBslnRst)
#define CapSense_BUT1_LOW_BSLN_RST_OFFSET                   (78u)
#define CapSense_BUT1_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT1_LOW_BSLN_RST_PARAM_ID                 (0x6D82004Eu)

#define CapSense_BUT1_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but1.bslnCoeff)
#define CapSense_BUT1_BSLN_COEFF_OFFSET                     (79u)
#define CapSense_BUT1_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT1_BSLN_COEFF_PARAM_ID                   (0x6B82004Fu)

#define CapSense_BUT1_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but1.idacMod[0u])
#define CapSense_BUT1_IDAC_MOD0_OFFSET                      (80u)
#define CapSense_BUT1_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT1_IDAC_MOD0_PARAM_ID                    (0x41020050u)

#define CapSense_BUT1_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but1.snsClk)
#define CapSense_BUT1_SNS_CLK_OFFSET                        (81u)
#define CapSense_BUT1_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT1_SNS_CLK_PARAM_ID                      (0x4A820051u)

#define CapSense_BUT1_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but1.snsClkSource)
#define CapSense_BUT1_SNS_CLK_SOURCE_OFFSET                 (82u)
#define CapSense_BUT1_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT1_SNS_CLK_SOURCE_PARAM_ID               (0x40820052u)

#define CapSense_BUT1_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but1.fingerCap)
#define CapSense_BUT1_FINGER_CAP_OFFSET                     (84u)
#define CapSense_BUT1_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT1_FINGER_CAP_PARAM_ID                   (0xA3020054u)

#define CapSense_BUT1_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but1.sigPFC)
#define CapSense_BUT1_SIGPFC_OFFSET                         (86u)
#define CapSense_BUT1_SIGPFC_SIZE                           (2u)
#define CapSense_BUT1_SIGPFC_PARAM_ID                       (0xAF020056u)

#define CapSense_BUT3_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but3.resolution)
#define CapSense_BUT3_RESOLUTION_OFFSET                     (88u)
#define CapSense_BUT3_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT3_RESOLUTION_PARAM_ID                   (0x85830058u)

#define CapSense_BUT3_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but3.fingerTh)
#define CapSense_BUT3_FINGER_TH_OFFSET                      (90u)
#define CapSense_BUT3_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT3_FINGER_TH_PARAM_ID                    (0xA283005Au)

#define CapSense_BUT3_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but3.noiseTh)
#define CapSense_BUT3_NOISE_TH_OFFSET                       (92u)
#define CapSense_BUT3_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT3_NOISE_TH_PARAM_ID                     (0x6783005Cu)

#define CapSense_BUT3_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but3.nNoiseTh)
#define CapSense_BUT3_NNOISE_TH_OFFSET                      (93u)
#define CapSense_BUT3_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT3_NNOISE_TH_PARAM_ID                    (0x6183005Du)

#define CapSense_BUT3_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but3.hysteresis)
#define CapSense_BUT3_HYSTERESIS_OFFSET                     (94u)
#define CapSense_BUT3_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT3_HYSTERESIS_PARAM_ID                   (0x6B83005Eu)

#define CapSense_BUT3_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but3.onDebounce)
#define CapSense_BUT3_ON_DEBOUNCE_OFFSET                    (95u)
#define CapSense_BUT3_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT3_ON_DEBOUNCE_PARAM_ID                  (0x6D83005Fu)

#define CapSense_BUT3_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but3.lowBslnRst)
#define CapSense_BUT3_LOW_BSLN_RST_OFFSET                   (96u)
#define CapSense_BUT3_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT3_LOW_BSLN_RST_PARAM_ID                 (0x6B830060u)

#define CapSense_BUT3_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but3.bslnCoeff)
#define CapSense_BUT3_BSLN_COEFF_OFFSET                     (97u)
#define CapSense_BUT3_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT3_BSLN_COEFF_PARAM_ID                   (0x6D830061u)

#define CapSense_BUT3_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but3.idacMod[0u])
#define CapSense_BUT3_IDAC_MOD0_OFFSET                      (98u)
#define CapSense_BUT3_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT3_IDAC_MOD0_PARAM_ID                    (0x41030062u)

#define CapSense_BUT3_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but3.snsClk)
#define CapSense_BUT3_SNS_CLK_OFFSET                        (99u)
#define CapSense_BUT3_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT3_SNS_CLK_PARAM_ID                      (0x4A830063u)

#define CapSense_BUT3_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but3.snsClkSource)
#define CapSense_BUT3_SNS_CLK_SOURCE_OFFSET                 (100u)
#define CapSense_BUT3_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT3_SNS_CLK_SOURCE_PARAM_ID               (0x41830064u)

#define CapSense_BUT3_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but3.fingerCap)
#define CapSense_BUT3_FINGER_CAP_OFFSET                     (102u)
#define CapSense_BUT3_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT3_FINGER_CAP_PARAM_ID                   (0xA3030066u)

#define CapSense_BUT3_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but3.sigPFC)
#define CapSense_BUT3_SIGPFC_OFFSET                         (104u)
#define CapSense_BUT3_SIGPFC_SIZE                           (2u)
#define CapSense_BUT3_SIGPFC_PARAM_ID                       (0xAC030068u)

#define CapSense_BUT4_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but4.resolution)
#define CapSense_BUT4_RESOLUTION_OFFSET                     (106u)
#define CapSense_BUT4_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT4_RESOLUTION_PARAM_ID                   (0x8F84006Au)

#define CapSense_BUT4_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but4.fingerTh)
#define CapSense_BUT4_FINGER_TH_OFFSET                      (108u)
#define CapSense_BUT4_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT4_FINGER_TH_PARAM_ID                    (0xA984006Cu)

#define CapSense_BUT4_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but4.noiseTh)
#define CapSense_BUT4_NOISE_TH_OFFSET                       (110u)
#define CapSense_BUT4_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT4_NOISE_TH_PARAM_ID                     (0x6D84006Eu)

#define CapSense_BUT4_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but4.nNoiseTh)
#define CapSense_BUT4_NNOISE_TH_OFFSET                      (111u)
#define CapSense_BUT4_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT4_NNOISE_TH_PARAM_ID                    (0x6B84006Fu)

#define CapSense_BUT4_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but4.hysteresis)
#define CapSense_BUT4_HYSTERESIS_OFFSET                     (112u)
#define CapSense_BUT4_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT4_HYSTERESIS_PARAM_ID                   (0x67840070u)

#define CapSense_BUT4_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but4.onDebounce)
#define CapSense_BUT4_ON_DEBOUNCE_OFFSET                    (113u)
#define CapSense_BUT4_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT4_ON_DEBOUNCE_PARAM_ID                  (0x61840071u)

#define CapSense_BUT4_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but4.lowBslnRst)
#define CapSense_BUT4_LOW_BSLN_RST_OFFSET                   (114u)
#define CapSense_BUT4_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT4_LOW_BSLN_RST_PARAM_ID                 (0x6B840072u)

#define CapSense_BUT4_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but4.bslnCoeff)
#define CapSense_BUT4_BSLN_COEFF_OFFSET                     (115u)
#define CapSense_BUT4_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT4_BSLN_COEFF_PARAM_ID                   (0x6D840073u)

#define CapSense_BUT4_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but4.idacMod[0u])
#define CapSense_BUT4_IDAC_MOD0_OFFSET                      (116u)
#define CapSense_BUT4_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT4_IDAC_MOD0_PARAM_ID                    (0x40040074u)

#define CapSense_BUT4_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but4.snsClk)
#define CapSense_BUT4_SNS_CLK_OFFSET                        (117u)
#define CapSense_BUT4_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT4_SNS_CLK_PARAM_ID                      (0x4B840075u)

#define CapSense_BUT4_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but4.snsClkSource)
#define CapSense_BUT4_SNS_CLK_SOURCE_OFFSET                 (118u)
#define CapSense_BUT4_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT4_SNS_CLK_SOURCE_PARAM_ID               (0x41840076u)

#define CapSense_BUT4_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but4.fingerCap)
#define CapSense_BUT4_FINGER_CAP_OFFSET                     (120u)
#define CapSense_BUT4_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT4_FINGER_CAP_PARAM_ID                   (0xA0040078u)

#define CapSense_BUT4_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but4.sigPFC)
#define CapSense_BUT4_SIGPFC_OFFSET                         (122u)
#define CapSense_BUT4_SIGPFC_SIZE                           (2u)
#define CapSense_BUT4_SIGPFC_PARAM_ID                       (0xAC04007Au)

#define CapSense_BUT5_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but5.resolution)
#define CapSense_BUT5_RESOLUTION_OFFSET                     (124u)
#define CapSense_BUT5_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT5_RESOLUTION_PARAM_ID                   (0x8485007Cu)

#define CapSense_BUT5_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but5.fingerTh)
#define CapSense_BUT5_FINGER_TH_OFFSET                      (126u)
#define CapSense_BUT5_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT5_FINGER_TH_PARAM_ID                    (0xA385007Eu)

#define CapSense_BUT5_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but5.noiseTh)
#define CapSense_BUT5_NOISE_TH_OFFSET                       (128u)
#define CapSense_BUT5_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT5_NOISE_TH_PARAM_ID                     (0x65850080u)

#define CapSense_BUT5_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but5.nNoiseTh)
#define CapSense_BUT5_NNOISE_TH_OFFSET                      (129u)
#define CapSense_BUT5_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT5_NNOISE_TH_PARAM_ID                    (0x63850081u)

#define CapSense_BUT5_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but5.hysteresis)
#define CapSense_BUT5_HYSTERESIS_OFFSET                     (130u)
#define CapSense_BUT5_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT5_HYSTERESIS_PARAM_ID                   (0x69850082u)

#define CapSense_BUT5_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but5.onDebounce)
#define CapSense_BUT5_ON_DEBOUNCE_OFFSET                    (131u)
#define CapSense_BUT5_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT5_ON_DEBOUNCE_PARAM_ID                  (0x6F850083u)

#define CapSense_BUT5_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but5.lowBslnRst)
#define CapSense_BUT5_LOW_BSLN_RST_OFFSET                   (132u)
#define CapSense_BUT5_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT5_LOW_BSLN_RST_PARAM_ID                 (0x64850084u)

#define CapSense_BUT5_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but5.bslnCoeff)
#define CapSense_BUT5_BSLN_COEFF_OFFSET                     (133u)
#define CapSense_BUT5_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT5_BSLN_COEFF_PARAM_ID                   (0x62850085u)

#define CapSense_BUT5_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but5.idacMod[0u])
#define CapSense_BUT5_IDAC_MOD0_OFFSET                      (134u)
#define CapSense_BUT5_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT5_IDAC_MOD0_PARAM_ID                    (0x4E050086u)

#define CapSense_BUT5_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but5.snsClk)
#define CapSense_BUT5_SNS_CLK_OFFSET                        (135u)
#define CapSense_BUT5_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT5_SNS_CLK_PARAM_ID                      (0x45850087u)

#define CapSense_BUT5_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but5.snsClkSource)
#define CapSense_BUT5_SNS_CLK_SOURCE_OFFSET                 (136u)
#define CapSense_BUT5_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT5_SNS_CLK_SOURCE_PARAM_ID               (0x4C850088u)

#define CapSense_BUT5_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but5.fingerCap)
#define CapSense_BUT5_FINGER_CAP_OFFSET                     (138u)
#define CapSense_BUT5_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT5_FINGER_CAP_PARAM_ID                   (0xAE05008Au)

#define CapSense_BUT5_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but5.sigPFC)
#define CapSense_BUT5_SIGPFC_OFFSET                         (140u)
#define CapSense_BUT5_SIGPFC_SIZE                           (2u)
#define CapSense_BUT5_SIGPFC_PARAM_ID                       (0xA305008Cu)

#define CapSense_BUT6_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but6.resolution)
#define CapSense_BUT6_RESOLUTION_OFFSET                     (142u)
#define CapSense_BUT6_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT6_RESOLUTION_PARAM_ID                   (0x8C86008Eu)

#define CapSense_BUT6_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but6.fingerTh)
#define CapSense_BUT6_FINGER_TH_OFFSET                      (144u)
#define CapSense_BUT6_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT6_FINGER_TH_PARAM_ID                    (0xAD860090u)

#define CapSense_BUT6_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but6.noiseTh)
#define CapSense_BUT6_NOISE_TH_OFFSET                       (146u)
#define CapSense_BUT6_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT6_NOISE_TH_PARAM_ID                     (0x69860092u)

#define CapSense_BUT6_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but6.nNoiseTh)
#define CapSense_BUT6_NNOISE_TH_OFFSET                      (147u)
#define CapSense_BUT6_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT6_NNOISE_TH_PARAM_ID                    (0x6F860093u)

#define CapSense_BUT6_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but6.hysteresis)
#define CapSense_BUT6_HYSTERESIS_OFFSET                     (148u)
#define CapSense_BUT6_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT6_HYSTERESIS_PARAM_ID                   (0x64860094u)

#define CapSense_BUT6_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but6.onDebounce)
#define CapSense_BUT6_ON_DEBOUNCE_OFFSET                    (149u)
#define CapSense_BUT6_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT6_ON_DEBOUNCE_PARAM_ID                  (0x62860095u)

#define CapSense_BUT6_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but6.lowBslnRst)
#define CapSense_BUT6_LOW_BSLN_RST_OFFSET                   (150u)
#define CapSense_BUT6_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT6_LOW_BSLN_RST_PARAM_ID                 (0x68860096u)

#define CapSense_BUT6_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but6.bslnCoeff)
#define CapSense_BUT6_BSLN_COEFF_OFFSET                     (151u)
#define CapSense_BUT6_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT6_BSLN_COEFF_PARAM_ID                   (0x6E860097u)

#define CapSense_BUT6_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but6.idacMod[0u])
#define CapSense_BUT6_IDAC_MOD0_OFFSET                      (152u)
#define CapSense_BUT6_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT6_IDAC_MOD0_PARAM_ID                    (0x41060098u)

#define CapSense_BUT6_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but6.snsClk)
#define CapSense_BUT6_SNS_CLK_OFFSET                        (153u)
#define CapSense_BUT6_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT6_SNS_CLK_PARAM_ID                      (0x4A860099u)

#define CapSense_BUT6_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but6.snsClkSource)
#define CapSense_BUT6_SNS_CLK_SOURCE_OFFSET                 (154u)
#define CapSense_BUT6_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT6_SNS_CLK_SOURCE_PARAM_ID               (0x4086009Au)

#define CapSense_BUT6_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but6.fingerCap)
#define CapSense_BUT6_FINGER_CAP_OFFSET                     (156u)
#define CapSense_BUT6_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT6_FINGER_CAP_PARAM_ID                   (0xA306009Cu)

#define CapSense_BUT6_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but6.sigPFC)
#define CapSense_BUT6_SIGPFC_OFFSET                         (158u)
#define CapSense_BUT6_SIGPFC_SIZE                           (2u)
#define CapSense_BUT6_SIGPFC_PARAM_ID                       (0xAF06009Eu)

#define CapSense_BUT8_RESOLUTION_VALUE                      (CapSense_dsRam.wdgtList.but8.resolution)
#define CapSense_BUT8_RESOLUTION_OFFSET                     (160u)
#define CapSense_BUT8_RESOLUTION_SIZE                       (2u)
#define CapSense_BUT8_RESOLUTION_PARAM_ID                   (0x8A8700A0u)

#define CapSense_BUT8_FINGER_TH_VALUE                       (CapSense_dsRam.wdgtList.but8.fingerTh)
#define CapSense_BUT8_FINGER_TH_OFFSET                      (162u)
#define CapSense_BUT8_FINGER_TH_SIZE                        (2u)
#define CapSense_BUT8_FINGER_TH_PARAM_ID                    (0xAD8700A2u)

#define CapSense_BUT8_NOISE_TH_VALUE                        (CapSense_dsRam.wdgtList.but8.noiseTh)
#define CapSense_BUT8_NOISE_TH_OFFSET                       (164u)
#define CapSense_BUT8_NOISE_TH_SIZE                         (1u)
#define CapSense_BUT8_NOISE_TH_PARAM_ID                     (0x688700A4u)

#define CapSense_BUT8_NNOISE_TH_VALUE                       (CapSense_dsRam.wdgtList.but8.nNoiseTh)
#define CapSense_BUT8_NNOISE_TH_OFFSET                      (165u)
#define CapSense_BUT8_NNOISE_TH_SIZE                        (1u)
#define CapSense_BUT8_NNOISE_TH_PARAM_ID                    (0x6E8700A5u)

#define CapSense_BUT8_HYSTERESIS_VALUE                      (CapSense_dsRam.wdgtList.but8.hysteresis)
#define CapSense_BUT8_HYSTERESIS_OFFSET                     (166u)
#define CapSense_BUT8_HYSTERESIS_SIZE                       (1u)
#define CapSense_BUT8_HYSTERESIS_PARAM_ID                   (0x648700A6u)

#define CapSense_BUT8_ON_DEBOUNCE_VALUE                     (CapSense_dsRam.wdgtList.but8.onDebounce)
#define CapSense_BUT8_ON_DEBOUNCE_OFFSET                    (167u)
#define CapSense_BUT8_ON_DEBOUNCE_SIZE                      (1u)
#define CapSense_BUT8_ON_DEBOUNCE_PARAM_ID                  (0x628700A7u)

#define CapSense_BUT8_LOW_BSLN_RST_VALUE                    (CapSense_dsRam.wdgtList.but8.lowBslnRst)
#define CapSense_BUT8_LOW_BSLN_RST_OFFSET                   (168u)
#define CapSense_BUT8_LOW_BSLN_RST_SIZE                     (1u)
#define CapSense_BUT8_LOW_BSLN_RST_PARAM_ID                 (0x6B8700A8u)

#define CapSense_BUT8_BSLN_COEFF_VALUE                      (CapSense_dsRam.wdgtList.but8.bslnCoeff)
#define CapSense_BUT8_BSLN_COEFF_OFFSET                     (169u)
#define CapSense_BUT8_BSLN_COEFF_SIZE                       (1u)
#define CapSense_BUT8_BSLN_COEFF_PARAM_ID                   (0x6D8700A9u)

#define CapSense_BUT8_IDAC_MOD0_VALUE                       (CapSense_dsRam.wdgtList.but8.idacMod[0u])
#define CapSense_BUT8_IDAC_MOD0_OFFSET                      (170u)
#define CapSense_BUT8_IDAC_MOD0_SIZE                        (1u)
#define CapSense_BUT8_IDAC_MOD0_PARAM_ID                    (0x410700AAu)

#define CapSense_BUT8_SNS_CLK_VALUE                         (CapSense_dsRam.wdgtList.but8.snsClk)
#define CapSense_BUT8_SNS_CLK_OFFSET                        (171u)
#define CapSense_BUT8_SNS_CLK_SIZE                          (1u)
#define CapSense_BUT8_SNS_CLK_PARAM_ID                      (0x4A8700ABu)

#define CapSense_BUT8_SNS_CLK_SOURCE_VALUE                  (CapSense_dsRam.wdgtList.but8.snsClkSource)
#define CapSense_BUT8_SNS_CLK_SOURCE_OFFSET                 (172u)
#define CapSense_BUT8_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapSense_BUT8_SNS_CLK_SOURCE_PARAM_ID               (0x418700ACu)

#define CapSense_BUT8_FINGER_CAP_VALUE                      (CapSense_dsRam.wdgtList.but8.fingerCap)
#define CapSense_BUT8_FINGER_CAP_OFFSET                     (174u)
#define CapSense_BUT8_FINGER_CAP_SIZE                       (2u)
#define CapSense_BUT8_FINGER_CAP_PARAM_ID                   (0xA30700AEu)

#define CapSense_BUT8_SIGPFC_VALUE                          (CapSense_dsRam.wdgtList.but8.sigPFC)
#define CapSense_BUT8_SIGPFC_OFFSET                         (176u)
#define CapSense_BUT8_SIGPFC_SIZE                           (2u)
#define CapSense_BUT8_SIGPFC_PARAM_ID                       (0xA90700B0u)

#define CapSense_LINEARSLIDER0_SNS0_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[0u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS0_RAW0_OFFSET             (178u)
#define CapSense_LINEARSLIDER0_SNS0_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS0_RAW0_PARAM_ID           (0x870000B2u)

#define CapSense_LINEARSLIDER0_SNS0_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[0u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS0_BSLN0_OFFSET            (180u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN0_PARAM_ID          (0x8A0000B4u)

#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[0u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_OFFSET        (182u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS0_BSLN_EXT0_PARAM_ID      (0x4E0000B6u)

#define CapSense_LINEARSLIDER0_SNS0_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[0u].diff)
#define CapSense_LINEARSLIDER0_SNS0_DIFF_OFFSET             (184u)
#define CapSense_LINEARSLIDER0_SNS0_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS0_DIFF_PARAM_ID           (0x890000B8u)

#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[0u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_OFFSET (186u)
#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID (0x4D0000BAu)

#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[0u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_OFFSET       (187u)
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0_PARAM_ID     (0x4B0000BBu)

#define CapSense_LINEARSLIDER0_SNS1_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[1u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS1_RAW0_OFFSET             (188u)
#define CapSense_LINEARSLIDER0_SNS1_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS1_RAW0_PARAM_ID           (0x880000BCu)

#define CapSense_LINEARSLIDER0_SNS1_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[1u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS1_BSLN0_OFFSET            (190u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN0_PARAM_ID          (0x840000BEu)

#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[1u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_OFFSET        (192u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS1_BSLN_EXT0_PARAM_ID      (0x410000C0u)

#define CapSense_LINEARSLIDER0_SNS1_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[1u].diff)
#define CapSense_LINEARSLIDER0_SNS1_DIFF_OFFSET             (194u)
#define CapSense_LINEARSLIDER0_SNS1_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS1_DIFF_PARAM_ID           (0x850000C2u)

#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[1u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_OFFSET (196u)
#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS1_NEG_BSLN_RST_CNT0_PARAM_ID (0x400000C4u)

#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[1u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_OFFSET       (197u)
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0_PARAM_ID     (0x460000C5u)

#define CapSense_LINEARSLIDER0_SNS2_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[2u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS2_RAW0_OFFSET             (198u)
#define CapSense_LINEARSLIDER0_SNS2_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS2_RAW0_PARAM_ID           (0x840000C6u)

#define CapSense_LINEARSLIDER0_SNS2_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[2u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS2_BSLN0_OFFSET            (200u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN0_PARAM_ID          (0x8B0000C8u)

#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[2u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_OFFSET        (202u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS2_BSLN_EXT0_PARAM_ID      (0x4F0000CAu)

#define CapSense_LINEARSLIDER0_SNS2_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[2u].diff)
#define CapSense_LINEARSLIDER0_SNS2_DIFF_OFFSET             (204u)
#define CapSense_LINEARSLIDER0_SNS2_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS2_DIFF_PARAM_ID           (0x8A0000CCu)

#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[2u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_OFFSET (206u)
#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS2_NEG_BSLN_RST_CNT0_PARAM_ID (0x4E0000CEu)

#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[2u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_OFFSET       (207u)
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0_PARAM_ID     (0x480000CFu)

#define CapSense_LINEARSLIDER0_SNS3_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[3u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS3_RAW0_OFFSET             (208u)
#define CapSense_LINEARSLIDER0_SNS3_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS3_RAW0_PARAM_ID           (0x8C0000D0u)

#define CapSense_LINEARSLIDER0_SNS3_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[3u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS3_BSLN0_OFFSET            (210u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN0_PARAM_ID          (0x800000D2u)

#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[3u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_OFFSET        (212u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS3_BSLN_EXT0_PARAM_ID      (0x450000D4u)

#define CapSense_LINEARSLIDER0_SNS3_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[3u].diff)
#define CapSense_LINEARSLIDER0_SNS3_DIFF_OFFSET             (214u)
#define CapSense_LINEARSLIDER0_SNS3_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS3_DIFF_PARAM_ID           (0x810000D6u)

#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[3u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_OFFSET (216u)
#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS3_NEG_BSLN_RST_CNT0_PARAM_ID (0x460000D8u)

#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[3u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_OFFSET       (217u)
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0_PARAM_ID     (0x400000D9u)

#define CapSense_LINEARSLIDER0_SNS4_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[4u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS4_RAW0_OFFSET             (218u)
#define CapSense_LINEARSLIDER0_SNS4_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS4_RAW0_PARAM_ID           (0x820000DAu)

#define CapSense_LINEARSLIDER0_SNS4_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[4u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS4_BSLN0_OFFSET            (220u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN0_PARAM_ID          (0x8F0000DCu)

#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[4u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_OFFSET        (222u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS4_BSLN_EXT0_PARAM_ID      (0x4B0000DEu)

#define CapSense_LINEARSLIDER0_SNS4_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[4u].diff)
#define CapSense_LINEARSLIDER0_SNS4_DIFF_OFFSET             (224u)
#define CapSense_LINEARSLIDER0_SNS4_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS4_DIFF_PARAM_ID           (0x830000E0u)

#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[4u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_OFFSET (226u)
#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS4_NEG_BSLN_RST_CNT0_PARAM_ID (0x470000E2u)

#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[4u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_OFFSET       (227u)
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0_PARAM_ID     (0x410000E3u)

#define CapSense_LINEARSLIDER0_SNS5_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[5u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS5_RAW0_OFFSET             (228u)
#define CapSense_LINEARSLIDER0_SNS5_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS5_RAW0_PARAM_ID           (0x820000E4u)

#define CapSense_LINEARSLIDER0_SNS5_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[5u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS5_BSLN0_OFFSET            (230u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN0_PARAM_ID          (0x8E0000E6u)

#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[5u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_OFFSET        (232u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS5_BSLN_EXT0_PARAM_ID      (0x490000E8u)

#define CapSense_LINEARSLIDER0_SNS5_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[5u].diff)
#define CapSense_LINEARSLIDER0_SNS5_DIFF_OFFSET             (234u)
#define CapSense_LINEARSLIDER0_SNS5_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS5_DIFF_PARAM_ID           (0x8D0000EAu)

#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[5u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_OFFSET (236u)
#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS5_NEG_BSLN_RST_CNT0_PARAM_ID (0x480000ECu)

#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[5u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_OFFSET       (237u)
#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0_PARAM_ID     (0x4E0000EDu)

#define CapSense_LINEARSLIDER0_SNS6_RAW0_VALUE              (CapSense_dsRam.snsList.linearslider0[6u].raw[0u])
#define CapSense_LINEARSLIDER0_SNS6_RAW0_OFFSET             (238u)
#define CapSense_LINEARSLIDER0_SNS6_RAW0_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS6_RAW0_PARAM_ID           (0x8C0000EEu)

#define CapSense_LINEARSLIDER0_SNS6_BSLN0_VALUE             (CapSense_dsRam.snsList.linearslider0[6u].bsln[0u])
#define CapSense_LINEARSLIDER0_SNS6_BSLN0_OFFSET            (240u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN0_SIZE              (2u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN0_PARAM_ID          (0x860000F0u)

#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_VALUE         (CapSense_dsRam.snsList.linearslider0[6u].bslnExt[0u])
#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_OFFSET        (242u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_SIZE          (1u)
#define CapSense_LINEARSLIDER0_SNS6_BSLN_EXT0_PARAM_ID      (0x420000F2u)

#define CapSense_LINEARSLIDER0_SNS6_DIFF_VALUE              (CapSense_dsRam.snsList.linearslider0[6u].diff)
#define CapSense_LINEARSLIDER0_SNS6_DIFF_OFFSET             (244u)
#define CapSense_LINEARSLIDER0_SNS6_DIFF_SIZE               (2u)
#define CapSense_LINEARSLIDER0_SNS6_DIFF_PARAM_ID           (0x870000F4u)

#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_VALUE (CapSense_dsRam.snsList.linearslider0[6u].negBslnRstCnt[0u])
#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_OFFSET (246u)
#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_SIZE  (1u)
#define CapSense_LINEARSLIDER0_SNS6_NEG_BSLN_RST_CNT0_PARAM_ID (0x430000F6u)

#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_VALUE        (CapSense_dsRam.snsList.linearslider0[6u].idacComp[0u])
#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_OFFSET       (247u)
#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_SIZE         (1u)
#define CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0_PARAM_ID     (0x450000F7u)

#define CapSense_BUT0_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but0[0u].raw[0u])
#define CapSense_BUT0_SNS0_RAW0_OFFSET                      (248u)
#define CapSense_BUT0_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT0_SNS0_RAW0_PARAM_ID                    (0x840000F8u)

#define CapSense_BUT0_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but0[0u].bsln[0u])
#define CapSense_BUT0_SNS0_BSLN0_OFFSET                     (250u)
#define CapSense_BUT0_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT0_SNS0_BSLN0_PARAM_ID                   (0x880000FAu)

#define CapSense_BUT0_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but0[0u].bslnExt[0u])
#define CapSense_BUT0_SNS0_BSLN_EXT0_OFFSET                 (252u)
#define CapSense_BUT0_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT0_SNS0_BSLN_EXT0_PARAM_ID               (0x4D0000FCu)

#define CapSense_BUT0_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but0[0u].diff)
#define CapSense_BUT0_SNS0_DIFF_OFFSET                      (254u)
#define CapSense_BUT0_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT0_SNS0_DIFF_PARAM_ID                    (0x890000FEu)

#define CapSense_BUT0_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but0[0u].negBslnRstCnt[0u])
#define CapSense_BUT0_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (256u)
#define CapSense_BUT0_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT0_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x44000100u)

#define CapSense_BUT0_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but0[0u].idacComp[0u])
#define CapSense_BUT0_SNS0_IDAC_COMP0_OFFSET                (257u)
#define CapSense_BUT0_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT0_SNS0_IDAC_COMP0_PARAM_ID              (0x42000101u)

#define CapSense_BUT1_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but1[0u].raw[0u])
#define CapSense_BUT1_SNS0_RAW0_OFFSET                      (258u)
#define CapSense_BUT1_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT1_SNS0_RAW0_PARAM_ID                    (0x80000102u)

#define CapSense_BUT1_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but1[0u].bsln[0u])
#define CapSense_BUT1_SNS0_BSLN0_OFFSET                     (260u)
#define CapSense_BUT1_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT1_SNS0_BSLN0_PARAM_ID                   (0x8D000104u)

#define CapSense_BUT1_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but1[0u].bslnExt[0u])
#define CapSense_BUT1_SNS0_BSLN_EXT0_OFFSET                 (262u)
#define CapSense_BUT1_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT1_SNS0_BSLN_EXT0_PARAM_ID               (0x49000106u)

#define CapSense_BUT1_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but1[0u].diff)
#define CapSense_BUT1_SNS0_DIFF_OFFSET                      (264u)
#define CapSense_BUT1_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT1_SNS0_DIFF_PARAM_ID                    (0x8E000108u)

#define CapSense_BUT1_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but1[0u].negBslnRstCnt[0u])
#define CapSense_BUT1_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (266u)
#define CapSense_BUT1_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT1_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x4A00010Au)

#define CapSense_BUT1_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but1[0u].idacComp[0u])
#define CapSense_BUT1_SNS0_IDAC_COMP0_OFFSET                (267u)
#define CapSense_BUT1_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT1_SNS0_IDAC_COMP0_PARAM_ID              (0x4C00010Bu)

#define CapSense_BUT3_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but3[0u].raw[0u])
#define CapSense_BUT3_SNS0_RAW0_OFFSET                      (268u)
#define CapSense_BUT3_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT3_SNS0_RAW0_PARAM_ID                    (0x8F00010Cu)

#define CapSense_BUT3_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but3[0u].bsln[0u])
#define CapSense_BUT3_SNS0_BSLN0_OFFSET                     (270u)
#define CapSense_BUT3_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT3_SNS0_BSLN0_PARAM_ID                   (0x8300010Eu)

#define CapSense_BUT3_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but3[0u].bslnExt[0u])
#define CapSense_BUT3_SNS0_BSLN_EXT0_OFFSET                 (272u)
#define CapSense_BUT3_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT3_SNS0_BSLN_EXT0_PARAM_ID               (0x41000110u)

#define CapSense_BUT3_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but3[0u].diff)
#define CapSense_BUT3_SNS0_DIFF_OFFSET                      (274u)
#define CapSense_BUT3_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT3_SNS0_DIFF_PARAM_ID                    (0x85000112u)

#define CapSense_BUT3_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but3[0u].negBslnRstCnt[0u])
#define CapSense_BUT3_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (276u)
#define CapSense_BUT3_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT3_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x40000114u)

#define CapSense_BUT3_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but3[0u].idacComp[0u])
#define CapSense_BUT3_SNS0_IDAC_COMP0_OFFSET                (277u)
#define CapSense_BUT3_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT3_SNS0_IDAC_COMP0_PARAM_ID              (0x46000115u)

#define CapSense_BUT4_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but4[0u].raw[0u])
#define CapSense_BUT4_SNS0_RAW0_OFFSET                      (278u)
#define CapSense_BUT4_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT4_SNS0_RAW0_PARAM_ID                    (0x84000116u)

#define CapSense_BUT4_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but4[0u].bsln[0u])
#define CapSense_BUT4_SNS0_BSLN0_OFFSET                     (280u)
#define CapSense_BUT4_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT4_SNS0_BSLN0_PARAM_ID                   (0x8B000118u)

#define CapSense_BUT4_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but4[0u].bslnExt[0u])
#define CapSense_BUT4_SNS0_BSLN_EXT0_OFFSET                 (282u)
#define CapSense_BUT4_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT4_SNS0_BSLN_EXT0_PARAM_ID               (0x4F00011Au)

#define CapSense_BUT4_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but4[0u].diff)
#define CapSense_BUT4_SNS0_DIFF_OFFSET                      (284u)
#define CapSense_BUT4_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT4_SNS0_DIFF_PARAM_ID                    (0x8A00011Cu)

#define CapSense_BUT4_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but4[0u].negBslnRstCnt[0u])
#define CapSense_BUT4_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (286u)
#define CapSense_BUT4_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT4_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x4E00011Eu)

#define CapSense_BUT4_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but4[0u].idacComp[0u])
#define CapSense_BUT4_SNS0_IDAC_COMP0_OFFSET                (287u)
#define CapSense_BUT4_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT4_SNS0_IDAC_COMP0_PARAM_ID              (0x4800011Fu)

#define CapSense_BUT5_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but5[0u].raw[0u])
#define CapSense_BUT5_SNS0_RAW0_OFFSET                      (288u)
#define CapSense_BUT5_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT5_SNS0_RAW0_PARAM_ID                    (0x86000120u)

#define CapSense_BUT5_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but5[0u].bsln[0u])
#define CapSense_BUT5_SNS0_BSLN0_OFFSET                     (290u)
#define CapSense_BUT5_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT5_SNS0_BSLN0_PARAM_ID                   (0x8A000122u)

#define CapSense_BUT5_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but5[0u].bslnExt[0u])
#define CapSense_BUT5_SNS0_BSLN_EXT0_OFFSET                 (292u)
#define CapSense_BUT5_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT5_SNS0_BSLN_EXT0_PARAM_ID               (0x4F000124u)

#define CapSense_BUT5_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but5[0u].diff)
#define CapSense_BUT5_SNS0_DIFF_OFFSET                      (294u)
#define CapSense_BUT5_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT5_SNS0_DIFF_PARAM_ID                    (0x8B000126u)

#define CapSense_BUT5_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but5[0u].negBslnRstCnt[0u])
#define CapSense_BUT5_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (296u)
#define CapSense_BUT5_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT5_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x4C000128u)

#define CapSense_BUT5_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but5[0u].idacComp[0u])
#define CapSense_BUT5_SNS0_IDAC_COMP0_OFFSET                (297u)
#define CapSense_BUT5_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT5_SNS0_IDAC_COMP0_PARAM_ID              (0x4A000129u)

#define CapSense_BUT6_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but6[0u].raw[0u])
#define CapSense_BUT6_SNS0_RAW0_OFFSET                      (298u)
#define CapSense_BUT6_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT6_SNS0_RAW0_PARAM_ID                    (0x8800012Au)

#define CapSense_BUT6_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but6[0u].bsln[0u])
#define CapSense_BUT6_SNS0_BSLN0_OFFSET                     (300u)
#define CapSense_BUT6_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT6_SNS0_BSLN0_PARAM_ID                   (0x8500012Cu)

#define CapSense_BUT6_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but6[0u].bslnExt[0u])
#define CapSense_BUT6_SNS0_BSLN_EXT0_OFFSET                 (302u)
#define CapSense_BUT6_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT6_SNS0_BSLN_EXT0_PARAM_ID               (0x4100012Eu)

#define CapSense_BUT6_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but6[0u].diff)
#define CapSense_BUT6_SNS0_DIFF_OFFSET                      (304u)
#define CapSense_BUT6_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT6_SNS0_DIFF_PARAM_ID                    (0x83000130u)

#define CapSense_BUT6_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but6[0u].negBslnRstCnt[0u])
#define CapSense_BUT6_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (306u)
#define CapSense_BUT6_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT6_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x47000132u)

#define CapSense_BUT6_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but6[0u].idacComp[0u])
#define CapSense_BUT6_SNS0_IDAC_COMP0_OFFSET                (307u)
#define CapSense_BUT6_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT6_SNS0_IDAC_COMP0_PARAM_ID              (0x41000133u)

#define CapSense_BUT8_SNS0_RAW0_VALUE                       (CapSense_dsRam.snsList.but8[0u].raw[0u])
#define CapSense_BUT8_SNS0_RAW0_OFFSET                      (308u)
#define CapSense_BUT8_SNS0_RAW0_SIZE                        (2u)
#define CapSense_BUT8_SNS0_RAW0_PARAM_ID                    (0x82000134u)

#define CapSense_BUT8_SNS0_BSLN0_VALUE                      (CapSense_dsRam.snsList.but8[0u].bsln[0u])
#define CapSense_BUT8_SNS0_BSLN0_OFFSET                     (310u)
#define CapSense_BUT8_SNS0_BSLN0_SIZE                       (2u)
#define CapSense_BUT8_SNS0_BSLN0_PARAM_ID                   (0x8E000136u)

#define CapSense_BUT8_SNS0_BSLN_EXT0_VALUE                  (CapSense_dsRam.snsList.but8[0u].bslnExt[0u])
#define CapSense_BUT8_SNS0_BSLN_EXT0_OFFSET                 (312u)
#define CapSense_BUT8_SNS0_BSLN_EXT0_SIZE                   (1u)
#define CapSense_BUT8_SNS0_BSLN_EXT0_PARAM_ID               (0x49000138u)

#define CapSense_BUT8_SNS0_DIFF_VALUE                       (CapSense_dsRam.snsList.but8[0u].diff)
#define CapSense_BUT8_SNS0_DIFF_OFFSET                      (314u)
#define CapSense_BUT8_SNS0_DIFF_SIZE                        (2u)
#define CapSense_BUT8_SNS0_DIFF_PARAM_ID                    (0x8D00013Au)

#define CapSense_BUT8_SNS0_NEG_BSLN_RST_CNT0_VALUE          (CapSense_dsRam.snsList.but8[0u].negBslnRstCnt[0u])
#define CapSense_BUT8_SNS0_NEG_BSLN_RST_CNT0_OFFSET         (316u)
#define CapSense_BUT8_SNS0_NEG_BSLN_RST_CNT0_SIZE           (1u)
#define CapSense_BUT8_SNS0_NEG_BSLN_RST_CNT0_PARAM_ID       (0x4800013Cu)

#define CapSense_BUT8_SNS0_IDAC_COMP0_VALUE                 (CapSense_dsRam.snsList.but8[0u].idacComp[0u])
#define CapSense_BUT8_SNS0_IDAC_COMP0_OFFSET                (317u)
#define CapSense_BUT8_SNS0_IDAC_COMP0_SIZE                  (1u)
#define CapSense_BUT8_SNS0_IDAC_COMP0_PARAM_ID              (0x4E00013Du)

#define CapSense_SNR_TEST_WIDGET_ID_VALUE                   (CapSense_dsRam.snrTestWidgetId)
#define CapSense_SNR_TEST_WIDGET_ID_OFFSET                  (318u)
#define CapSense_SNR_TEST_WIDGET_ID_SIZE                    (1u)
#define CapSense_SNR_TEST_WIDGET_ID_PARAM_ID                (0x6F00013Eu)

#define CapSense_SNR_TEST_SENSOR_ID_VALUE                   (CapSense_dsRam.snrTestSensorId)
#define CapSense_SNR_TEST_SENSOR_ID_OFFSET                  (319u)
#define CapSense_SNR_TEST_SENSOR_ID_SIZE                    (1u)
#define CapSense_SNR_TEST_SENSOR_ID_PARAM_ID                (0x6900013Fu)

#define CapSense_SNR_TEST_SCAN_COUNTER_VALUE                (CapSense_dsRam.snrTestScanCounter)
#define CapSense_SNR_TEST_SCAN_COUNTER_OFFSET               (320u)
#define CapSense_SNR_TEST_SCAN_COUNTER_SIZE                 (2u)
#define CapSense_SNR_TEST_SCAN_COUNTER_PARAM_ID             (0x81000140u)

#define CapSense_SNR_TEST_RAW_COUNT0_VALUE                  (CapSense_dsRam.snrTestRawCount[0u])
#define CapSense_SNR_TEST_RAW_COUNT0_OFFSET                 (322u)
#define CapSense_SNR_TEST_RAW_COUNT0_SIZE                   (2u)
#define CapSense_SNR_TEST_RAW_COUNT0_PARAM_ID               (0x8D000142u)


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

#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_VALUE        (CapSense_dsFlash.wdgtArray[0].ptr2PosHistory)
#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_OFFSET       (32u)
#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_SIZE         (4u)
#define CapSense_LINEARSLIDER0_PTR2POS_HISTORY_PARAM_ID     (0xDB000020u)

#define CapSense_BUT0_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[1].ptr2SnsFlash)
#define CapSense_BUT0_PTR2SNS_FLASH_OFFSET                  (36u)
#define CapSense_BUT0_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT0_PTR2SNS_FLASH_PARAM_ID                (0xD9010024u)

#define CapSense_BUT0_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[1].ptr2WdgtRam)
#define CapSense_BUT0_PTR2WD_RAM_OFFSET                     (40u)
#define CapSense_BUT0_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT0_PTR2WD_RAM_PARAM_ID                   (0xDA010028u)

#define CapSense_BUT0_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[1].ptr2SnsRam)
#define CapSense_BUT0_PTR2SNS_RAM_OFFSET                    (44u)
#define CapSense_BUT0_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT0_PTR2SNS_RAM_PARAM_ID                  (0xDB01002Cu)

#define CapSense_BUT0_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[1].ptr2FltrHistory)
#define CapSense_BUT0_PTR2FLTR_HISTORY_OFFSET               (48u)
#define CapSense_BUT0_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT0_PTR2FLTR_HISTORY_PARAM_ID             (0xDD010030u)

#define CapSense_BUT0_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[1].ptr2DebounceArr)
#define CapSense_BUT0_PTR2DEBOUNCE_OFFSET                   (52u)
#define CapSense_BUT0_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT0_PTR2DEBOUNCE_PARAM_ID                 (0xDC010034u)

#define CapSense_BUT0_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[1].staticConfig)
#define CapSense_BUT0_STATIC_CONFIG_OFFSET                  (56u)
#define CapSense_BUT0_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT0_STATIC_CONFIG_PARAM_ID                (0x90010038u)

#define CapSense_BUT0_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[1].totalNumSns)
#define CapSense_BUT0_TOTAL_NUM_SNS_OFFSET                  (58u)
#define CapSense_BUT0_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT0_TOTAL_NUM_SNS_PARAM_ID                (0x9C01003Au)

#define CapSense_BUT0_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[1].wdgtType)
#define CapSense_BUT0_TYPE_OFFSET                           (60u)
#define CapSense_BUT0_TYPE_SIZE                             (1u)
#define CapSense_BUT0_TYPE_PARAM_ID                         (0x5901003Cu)

#define CapSense_BUT0_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[1].numCols)
#define CapSense_BUT0_NUM_COLS_OFFSET                       (61u)
#define CapSense_BUT0_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT0_NUM_COLS_PARAM_ID                     (0x5F01003Du)

#define CapSense_BUT0_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[1].xResolution)
#define CapSense_BUT0_X_RESOLUTION_OFFSET                   (62u)
#define CapSense_BUT0_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT0_X_RESOLUTION_PARAM_ID                 (0x9D01003Eu)

#define CapSense_BUT0_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[1].xCentroidMultiplier)
#define CapSense_BUT0_X_CENT_MULT_OFFSET                    (64u)
#define CapSense_BUT0_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT0_X_CENT_MULT_PARAM_ID                  (0xDF010040u)

#define CapSense_BUT0_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[1].ptr2PosHistory)
#define CapSense_BUT0_PTR2POS_HISTORY_OFFSET                (68u)
#define CapSense_BUT0_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT0_PTR2POS_HISTORY_PARAM_ID              (0xDE010044u)

#define CapSense_BUT1_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[2].ptr2SnsFlash)
#define CapSense_BUT1_PTR2SNS_FLASH_OFFSET                  (72u)
#define CapSense_BUT1_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT1_PTR2SNS_FLASH_PARAM_ID                (0xD8020048u)

#define CapSense_BUT1_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[2].ptr2WdgtRam)
#define CapSense_BUT1_PTR2WD_RAM_OFFSET                     (76u)
#define CapSense_BUT1_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT1_PTR2WD_RAM_PARAM_ID                   (0xD902004Cu)

#define CapSense_BUT1_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[2].ptr2SnsRam)
#define CapSense_BUT1_PTR2SNS_RAM_OFFSET                    (80u)
#define CapSense_BUT1_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT1_PTR2SNS_RAM_PARAM_ID                  (0xDF020050u)

#define CapSense_BUT1_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[2].ptr2FltrHistory)
#define CapSense_BUT1_PTR2FLTR_HISTORY_OFFSET               (84u)
#define CapSense_BUT1_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT1_PTR2FLTR_HISTORY_PARAM_ID             (0xDE020054u)

#define CapSense_BUT1_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[2].ptr2DebounceArr)
#define CapSense_BUT1_PTR2DEBOUNCE_OFFSET                   (88u)
#define CapSense_BUT1_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT1_PTR2DEBOUNCE_PARAM_ID                 (0xDD020058u)

#define CapSense_BUT1_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[2].staticConfig)
#define CapSense_BUT1_STATIC_CONFIG_OFFSET                  (92u)
#define CapSense_BUT1_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT1_STATIC_CONFIG_PARAM_ID                (0x9302005Cu)

#define CapSense_BUT1_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[2].totalNumSns)
#define CapSense_BUT1_TOTAL_NUM_SNS_OFFSET                  (94u)
#define CapSense_BUT1_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT1_TOTAL_NUM_SNS_PARAM_ID                (0x9F02005Eu)

#define CapSense_BUT1_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[2].wdgtType)
#define CapSense_BUT1_TYPE_OFFSET                           (96u)
#define CapSense_BUT1_TYPE_SIZE                             (1u)
#define CapSense_BUT1_TYPE_PARAM_ID                         (0x57020060u)

#define CapSense_BUT1_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[2].numCols)
#define CapSense_BUT1_NUM_COLS_OFFSET                       (97u)
#define CapSense_BUT1_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT1_NUM_COLS_PARAM_ID                     (0x51020061u)

#define CapSense_BUT1_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[2].xResolution)
#define CapSense_BUT1_X_RESOLUTION_OFFSET                   (98u)
#define CapSense_BUT1_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT1_X_RESOLUTION_PARAM_ID                 (0x93020062u)

#define CapSense_BUT1_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[2].xCentroidMultiplier)
#define CapSense_BUT1_X_CENT_MULT_OFFSET                    (100u)
#define CapSense_BUT1_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT1_X_CENT_MULT_PARAM_ID                  (0xD1020064u)

#define CapSense_BUT1_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[2].ptr2PosHistory)
#define CapSense_BUT1_PTR2POS_HISTORY_OFFSET                (104u)
#define CapSense_BUT1_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT1_PTR2POS_HISTORY_PARAM_ID              (0xD2020068u)

#define CapSense_BUT3_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[3].ptr2SnsFlash)
#define CapSense_BUT3_PTR2SNS_FLASH_OFFSET                  (108u)
#define CapSense_BUT3_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT3_PTR2SNS_FLASH_PARAM_ID                (0xD003006Cu)

#define CapSense_BUT3_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[3].ptr2WdgtRam)
#define CapSense_BUT3_PTR2WD_RAM_OFFSET                     (112u)
#define CapSense_BUT3_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT3_PTR2WD_RAM_PARAM_ID                   (0xD6030070u)

#define CapSense_BUT3_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[3].ptr2SnsRam)
#define CapSense_BUT3_PTR2SNS_RAM_OFFSET                    (116u)
#define CapSense_BUT3_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT3_PTR2SNS_RAM_PARAM_ID                  (0xD7030074u)

#define CapSense_BUT3_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[3].ptr2FltrHistory)
#define CapSense_BUT3_PTR2FLTR_HISTORY_OFFSET               (120u)
#define CapSense_BUT3_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT3_PTR2FLTR_HISTORY_PARAM_ID             (0xD4030078u)

#define CapSense_BUT3_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[3].ptr2DebounceArr)
#define CapSense_BUT3_PTR2DEBOUNCE_OFFSET                   (124u)
#define CapSense_BUT3_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT3_PTR2DEBOUNCE_PARAM_ID                 (0xD503007Cu)

#define CapSense_BUT3_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[3].staticConfig)
#define CapSense_BUT3_STATIC_CONFIG_OFFSET                  (128u)
#define CapSense_BUT3_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT3_STATIC_CONFIG_PARAM_ID                (0x98030080u)

#define CapSense_BUT3_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[3].totalNumSns)
#define CapSense_BUT3_TOTAL_NUM_SNS_OFFSET                  (130u)
#define CapSense_BUT3_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT3_TOTAL_NUM_SNS_PARAM_ID                (0x94030082u)

#define CapSense_BUT3_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[3].wdgtType)
#define CapSense_BUT3_TYPE_OFFSET                           (132u)
#define CapSense_BUT3_TYPE_SIZE                             (1u)
#define CapSense_BUT3_TYPE_PARAM_ID                         (0x51030084u)

#define CapSense_BUT3_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[3].numCols)
#define CapSense_BUT3_NUM_COLS_OFFSET                       (133u)
#define CapSense_BUT3_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT3_NUM_COLS_PARAM_ID                     (0x57030085u)

#define CapSense_BUT3_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[3].xResolution)
#define CapSense_BUT3_X_RESOLUTION_OFFSET                   (134u)
#define CapSense_BUT3_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT3_X_RESOLUTION_PARAM_ID                 (0x95030086u)

#define CapSense_BUT3_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[3].xCentroidMultiplier)
#define CapSense_BUT3_X_CENT_MULT_OFFSET                    (136u)
#define CapSense_BUT3_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT3_X_CENT_MULT_PARAM_ID                  (0xD5030088u)

#define CapSense_BUT3_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[3].ptr2PosHistory)
#define CapSense_BUT3_PTR2POS_HISTORY_OFFSET                (140u)
#define CapSense_BUT3_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT3_PTR2POS_HISTORY_PARAM_ID              (0xD403008Cu)

#define CapSense_BUT4_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[4].ptr2SnsFlash)
#define CapSense_BUT4_PTR2SNS_FLASH_OFFSET                  (144u)
#define CapSense_BUT4_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT4_PTR2SNS_FLASH_PARAM_ID                (0xDB040090u)

#define CapSense_BUT4_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[4].ptr2WdgtRam)
#define CapSense_BUT4_PTR2WD_RAM_OFFSET                     (148u)
#define CapSense_BUT4_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT4_PTR2WD_RAM_PARAM_ID                   (0xDA040094u)

#define CapSense_BUT4_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[4].ptr2SnsRam)
#define CapSense_BUT4_PTR2SNS_RAM_OFFSET                    (152u)
#define CapSense_BUT4_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT4_PTR2SNS_RAM_PARAM_ID                  (0xD9040098u)

#define CapSense_BUT4_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[4].ptr2FltrHistory)
#define CapSense_BUT4_PTR2FLTR_HISTORY_OFFSET               (156u)
#define CapSense_BUT4_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT4_PTR2FLTR_HISTORY_PARAM_ID             (0xD804009Cu)

#define CapSense_BUT4_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[4].ptr2DebounceArr)
#define CapSense_BUT4_PTR2DEBOUNCE_OFFSET                   (160u)
#define CapSense_BUT4_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT4_PTR2DEBOUNCE_PARAM_ID                 (0xD40400A0u)

#define CapSense_BUT4_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[4].staticConfig)
#define CapSense_BUT4_STATIC_CONFIG_OFFSET                  (164u)
#define CapSense_BUT4_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT4_STATIC_CONFIG_PARAM_ID                (0x9A0400A4u)

#define CapSense_BUT4_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[4].totalNumSns)
#define CapSense_BUT4_TOTAL_NUM_SNS_OFFSET                  (166u)
#define CapSense_BUT4_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT4_TOTAL_NUM_SNS_PARAM_ID                (0x960400A6u)

#define CapSense_BUT4_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[4].wdgtType)
#define CapSense_BUT4_TYPE_OFFSET                           (168u)
#define CapSense_BUT4_TYPE_SIZE                             (1u)
#define CapSense_BUT4_TYPE_PARAM_ID                         (0x510400A8u)

#define CapSense_BUT4_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[4].numCols)
#define CapSense_BUT4_NUM_COLS_OFFSET                       (169u)
#define CapSense_BUT4_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT4_NUM_COLS_PARAM_ID                     (0x570400A9u)

#define CapSense_BUT4_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[4].xResolution)
#define CapSense_BUT4_X_RESOLUTION_OFFSET                   (170u)
#define CapSense_BUT4_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT4_X_RESOLUTION_PARAM_ID                 (0x950400AAu)

#define CapSense_BUT4_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[4].xCentroidMultiplier)
#define CapSense_BUT4_X_CENT_MULT_OFFSET                    (172u)
#define CapSense_BUT4_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT4_X_CENT_MULT_PARAM_ID                  (0xD70400ACu)

#define CapSense_BUT4_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[4].ptr2PosHistory)
#define CapSense_BUT4_PTR2POS_HISTORY_OFFSET                (176u)
#define CapSense_BUT4_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT4_PTR2POS_HISTORY_PARAM_ID              (0xD10400B0u)

#define CapSense_BUT5_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[5].ptr2SnsFlash)
#define CapSense_BUT5_PTR2SNS_FLASH_OFFSET                  (180u)
#define CapSense_BUT5_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT5_PTR2SNS_FLASH_PARAM_ID                (0xD30500B4u)

#define CapSense_BUT5_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[5].ptr2WdgtRam)
#define CapSense_BUT5_PTR2WD_RAM_OFFSET                     (184u)
#define CapSense_BUT5_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT5_PTR2WD_RAM_PARAM_ID                   (0xD00500B8u)

#define CapSense_BUT5_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[5].ptr2SnsRam)
#define CapSense_BUT5_PTR2SNS_RAM_OFFSET                    (188u)
#define CapSense_BUT5_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT5_PTR2SNS_RAM_PARAM_ID                  (0xD10500BCu)

#define CapSense_BUT5_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[5].ptr2FltrHistory)
#define CapSense_BUT5_PTR2FLTR_HISTORY_OFFSET               (192u)
#define CapSense_BUT5_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT5_PTR2FLTR_HISTORY_PARAM_ID             (0xD00500C0u)

#define CapSense_BUT5_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[5].ptr2DebounceArr)
#define CapSense_BUT5_PTR2DEBOUNCE_OFFSET                   (196u)
#define CapSense_BUT5_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT5_PTR2DEBOUNCE_PARAM_ID                 (0xD10500C4u)

#define CapSense_BUT5_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[5].staticConfig)
#define CapSense_BUT5_STATIC_CONFIG_OFFSET                  (200u)
#define CapSense_BUT5_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT5_STATIC_CONFIG_PARAM_ID                (0x9D0500C8u)

#define CapSense_BUT5_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[5].totalNumSns)
#define CapSense_BUT5_TOTAL_NUM_SNS_OFFSET                  (202u)
#define CapSense_BUT5_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT5_TOTAL_NUM_SNS_PARAM_ID                (0x910500CAu)

#define CapSense_BUT5_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[5].wdgtType)
#define CapSense_BUT5_TYPE_OFFSET                           (204u)
#define CapSense_BUT5_TYPE_SIZE                             (1u)
#define CapSense_BUT5_TYPE_PARAM_ID                         (0x540500CCu)

#define CapSense_BUT5_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[5].numCols)
#define CapSense_BUT5_NUM_COLS_OFFSET                       (205u)
#define CapSense_BUT5_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT5_NUM_COLS_PARAM_ID                     (0x520500CDu)

#define CapSense_BUT5_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[5].xResolution)
#define CapSense_BUT5_X_RESOLUTION_OFFSET                   (206u)
#define CapSense_BUT5_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT5_X_RESOLUTION_PARAM_ID                 (0x900500CEu)

#define CapSense_BUT5_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[5].xCentroidMultiplier)
#define CapSense_BUT5_X_CENT_MULT_OFFSET                    (208u)
#define CapSense_BUT5_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT5_X_CENT_MULT_PARAM_ID                  (0xD50500D0u)

#define CapSense_BUT5_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[5].ptr2PosHistory)
#define CapSense_BUT5_PTR2POS_HISTORY_OFFSET                (212u)
#define CapSense_BUT5_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT5_PTR2POS_HISTORY_PARAM_ID              (0xD40500D4u)

#define CapSense_BUT6_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[6].ptr2SnsFlash)
#define CapSense_BUT6_PTR2SNS_FLASH_OFFSET                  (216u)
#define CapSense_BUT6_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT6_PTR2SNS_FLASH_PARAM_ID                (0xD20600D8u)

#define CapSense_BUT6_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[6].ptr2WdgtRam)
#define CapSense_BUT6_PTR2WD_RAM_OFFSET                     (220u)
#define CapSense_BUT6_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT6_PTR2WD_RAM_PARAM_ID                   (0xD30600DCu)

#define CapSense_BUT6_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[6].ptr2SnsRam)
#define CapSense_BUT6_PTR2SNS_RAM_OFFSET                    (224u)
#define CapSense_BUT6_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT6_PTR2SNS_RAM_PARAM_ID                  (0xDF0600E0u)

#define CapSense_BUT6_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[6].ptr2FltrHistory)
#define CapSense_BUT6_PTR2FLTR_HISTORY_OFFSET               (228u)
#define CapSense_BUT6_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT6_PTR2FLTR_HISTORY_PARAM_ID             (0xDE0600E4u)

#define CapSense_BUT6_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[6].ptr2DebounceArr)
#define CapSense_BUT6_PTR2DEBOUNCE_OFFSET                   (232u)
#define CapSense_BUT6_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT6_PTR2DEBOUNCE_PARAM_ID                 (0xDD0600E8u)

#define CapSense_BUT6_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[6].staticConfig)
#define CapSense_BUT6_STATIC_CONFIG_OFFSET                  (236u)
#define CapSense_BUT6_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT6_STATIC_CONFIG_PARAM_ID                (0x930600ECu)

#define CapSense_BUT6_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[6].totalNumSns)
#define CapSense_BUT6_TOTAL_NUM_SNS_OFFSET                  (238u)
#define CapSense_BUT6_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT6_TOTAL_NUM_SNS_PARAM_ID                (0x9F0600EEu)

#define CapSense_BUT6_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[6].wdgtType)
#define CapSense_BUT6_TYPE_OFFSET                           (240u)
#define CapSense_BUT6_TYPE_SIZE                             (1u)
#define CapSense_BUT6_TYPE_PARAM_ID                         (0x5D0600F0u)

#define CapSense_BUT6_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[6].numCols)
#define CapSense_BUT6_NUM_COLS_OFFSET                       (241u)
#define CapSense_BUT6_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT6_NUM_COLS_PARAM_ID                     (0x5B0600F1u)

#define CapSense_BUT6_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[6].xResolution)
#define CapSense_BUT6_X_RESOLUTION_OFFSET                   (242u)
#define CapSense_BUT6_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT6_X_RESOLUTION_PARAM_ID                 (0x990600F2u)

#define CapSense_BUT6_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[6].xCentroidMultiplier)
#define CapSense_BUT6_X_CENT_MULT_OFFSET                    (244u)
#define CapSense_BUT6_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT6_X_CENT_MULT_PARAM_ID                  (0xDB0600F4u)

#define CapSense_BUT6_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[6].ptr2PosHistory)
#define CapSense_BUT6_PTR2POS_HISTORY_OFFSET                (248u)
#define CapSense_BUT6_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT6_PTR2POS_HISTORY_PARAM_ID              (0xD80600F8u)

#define CapSense_BUT8_PTR2SNS_FLASH_VALUE                   (CapSense_dsFlash.wdgtArray[7].ptr2SnsFlash)
#define CapSense_BUT8_PTR2SNS_FLASH_OFFSET                  (252u)
#define CapSense_BUT8_PTR2SNS_FLASH_SIZE                    (4u)
#define CapSense_BUT8_PTR2SNS_FLASH_PARAM_ID                (0xDA0700FCu)

#define CapSense_BUT8_PTR2WD_RAM_VALUE                      (CapSense_dsFlash.wdgtArray[7].ptr2WdgtRam)
#define CapSense_BUT8_PTR2WD_RAM_OFFSET                     (256u)
#define CapSense_BUT8_PTR2WD_RAM_SIZE                       (4u)
#define CapSense_BUT8_PTR2WD_RAM_PARAM_ID                   (0xD3070100u)

#define CapSense_BUT8_PTR2SNS_RAM_VALUE                     (CapSense_dsFlash.wdgtArray[7].ptr2SnsRam)
#define CapSense_BUT8_PTR2SNS_RAM_OFFSET                    (260u)
#define CapSense_BUT8_PTR2SNS_RAM_SIZE                      (4u)
#define CapSense_BUT8_PTR2SNS_RAM_PARAM_ID                  (0xD2070104u)

#define CapSense_BUT8_PTR2FLTR_HISTORY_VALUE                (CapSense_dsFlash.wdgtArray[7].ptr2FltrHistory)
#define CapSense_BUT8_PTR2FLTR_HISTORY_OFFSET               (264u)
#define CapSense_BUT8_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapSense_BUT8_PTR2FLTR_HISTORY_PARAM_ID             (0xD1070108u)

#define CapSense_BUT8_PTR2DEBOUNCE_VALUE                    (CapSense_dsFlash.wdgtArray[7].ptr2DebounceArr)
#define CapSense_BUT8_PTR2DEBOUNCE_OFFSET                   (268u)
#define CapSense_BUT8_PTR2DEBOUNCE_SIZE                     (4u)
#define CapSense_BUT8_PTR2DEBOUNCE_PARAM_ID                 (0xD007010Cu)

#define CapSense_BUT8_STATIC_CONFIG_VALUE                   (CapSense_dsFlash.wdgtArray[7].staticConfig)
#define CapSense_BUT8_STATIC_CONFIG_OFFSET                  (272u)
#define CapSense_BUT8_STATIC_CONFIG_SIZE                    (2u)
#define CapSense_BUT8_STATIC_CONFIG_PARAM_ID                (0x99070110u)

#define CapSense_BUT8_TOTAL_NUM_SNS_VALUE                   (CapSense_dsFlash.wdgtArray[7].totalNumSns)
#define CapSense_BUT8_TOTAL_NUM_SNS_OFFSET                  (274u)
#define CapSense_BUT8_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapSense_BUT8_TOTAL_NUM_SNS_PARAM_ID                (0x95070112u)

#define CapSense_BUT8_TYPE_VALUE                            (CapSense_dsFlash.wdgtArray[7].wdgtType)
#define CapSense_BUT8_TYPE_OFFSET                           (276u)
#define CapSense_BUT8_TYPE_SIZE                             (1u)
#define CapSense_BUT8_TYPE_PARAM_ID                         (0x50070114u)

#define CapSense_BUT8_NUM_COLS_VALUE                        (CapSense_dsFlash.wdgtArray[7].numCols)
#define CapSense_BUT8_NUM_COLS_OFFSET                       (277u)
#define CapSense_BUT8_NUM_COLS_SIZE                         (1u)
#define CapSense_BUT8_NUM_COLS_PARAM_ID                     (0x56070115u)

#define CapSense_BUT8_X_RESOLUTION_VALUE                    (CapSense_dsFlash.wdgtArray[7].xResolution)
#define CapSense_BUT8_X_RESOLUTION_OFFSET                   (278u)
#define CapSense_BUT8_X_RESOLUTION_SIZE                     (2u)
#define CapSense_BUT8_X_RESOLUTION_PARAM_ID                 (0x94070116u)

#define CapSense_BUT8_X_CENT_MULT_VALUE                     (CapSense_dsFlash.wdgtArray[7].xCentroidMultiplier)
#define CapSense_BUT8_X_CENT_MULT_OFFSET                    (280u)
#define CapSense_BUT8_X_CENT_MULT_SIZE                      (4u)
#define CapSense_BUT8_X_CENT_MULT_PARAM_ID                  (0xD4070118u)

#define CapSense_BUT8_PTR2POS_HISTORY_VALUE                 (CapSense_dsFlash.wdgtArray[7].ptr2PosHistory)
#define CapSense_BUT8_PTR2POS_HISTORY_OFFSET                (284u)
#define CapSense_BUT8_PTR2POS_HISTORY_SIZE                  (4u)
#define CapSense_BUT8_PTR2POS_HISTORY_PARAM_ID              (0xD507011Cu)


#endif /* End CY_CAPSENSE_CapSense_REGISTER_MAP_H */

/* [] END OF FILE */
