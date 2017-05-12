/***************************************************************************//**
* \file CapSense_Structure.c
* \version 3.10
*
* \brief
*   This file defines the data structure global variables and provides implementation
*   for the high-level and low-level APIs of the Data Structure module.
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

#include <cytypes.h>
#include "CyLib.h"
#include "CapSense_Structure.h"
#include "CapSense_Configuration.h"

#if (0u != CapSense_ADC_EN)
    #include "CapSense_Adc.h"
#endif /* (0u != CapSense_ADC_EN) */

#if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
    #include "CapSense_SelfTest.h"
#endif

/*******************************************************************************
* Define the RAM Data Structure variables and their init data in flash
*******************************************************************************/
/**
* \if SECTION_GLOBAL_VARIABLES
* \addtogroup group_global_variables
* \{
*/

/**
* Variable that contains CapSense configuration, settings and scanning results.
* CapSense_dsRam represents RAM Data Structure.
*/
CapSense_RAM_STRUCT CapSense_dsRam;
/** \}
* \endif */

/*******************************************************************************
* Declare Widget's De-bounce Counters
*******************************************************************************/
static uint8 CapSense_debounceLinearSlider0[1u];
static uint8 CapSense_debounceBUT0[CapSense_BUT0_NUM_SENSORS];
static uint8 CapSense_debounceBUT1[CapSense_BUT1_NUM_SENSORS];
static uint8 CapSense_debounceBUT3[CapSense_BUT3_NUM_SENSORS];
static uint8 CapSense_debounceBUT4[CapSense_BUT4_NUM_SENSORS];
static uint8 CapSense_debounceBUT5[CapSense_BUT5_NUM_SENSORS];
static uint8 CapSense_debounceBUT6[CapSense_BUT6_NUM_SENSORS];
static uint8 CapSense_debounceBUT8[CapSense_BUT8_NUM_SENSORS];


/***************************************************************************//**
* Declare Filter History Objects
*******************************************************************************/
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryLinearSlider0[CapSense_LINEARSLIDER0_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT0[CapSense_BUT0_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT1[CapSense_BUT1_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT3[CapSense_BUT3_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT4[CapSense_BUT4_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT5[CapSense_BUT5_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT6[CapSense_BUT6_NUM_SENSORS];
static CapSense_REGULAR_FLTR_STRUCT CapSense_fltrHistoryBUT8[CapSense_BUT8_NUM_SENSORS];

/***************************************************************************//**
* Centroid positions history
*******************************************************************************/
static CapSense_SLIDER_POS_HISTORY_STRUCT CapSense_posHistoryLinearSlider0[CapSense_NUM_CENTROIDS];

/*******************************************************************************
* Define and initialize the Flash Data Structure
*******************************************************************************/

/**
* \if SECTION_API_CONSTANTS
* \addtogroup group_api_constants
* \{
*/

/**
* Constant for the FLASH Data Structure
*/
const CapSense_FLASH_STRUCT CapSense_dsFlash =
{
    /* Flash Widget Initialization Data */
    {
        { /* LinearSlider0 */
            &CapSense_ioList[0u],
            (void *)&CapSense_dsRam.wdgtList.linearslider0,
            CapSense_dsRam.snsList.linearslider0,
            (void *)CapSense_fltrHistoryLinearSlider0,
            CapSense_debounceLinearSlider0,
            CapSense_LINEARSLIDER0_STATIC_CONFIG,
            CapSense_LINEARSLIDER0_NUM_SENSORS,
            (uint8)CapSense_WD_LINEAR_SLIDER_E,
            CapSense_LINEARSLIDER0_NUM_SENSORS,
            CapSense_LINEARSLIDER0_X_RESOLUTION,
            CapSense_LINEARSLIDER0_X_CENT_MULT,
            (void *)&CapSense_posHistoryLinearSlider0,
        },
        { /* BUT0 */
            &CapSense_ioList[7u],
            (void *)&CapSense_dsRam.wdgtList.but0,
            CapSense_dsRam.snsList.but0,
            (void *)CapSense_fltrHistoryBUT0,
            CapSense_debounceBUT0,
            CapSense_BUT0_STATIC_CONFIG,
            CapSense_BUT0_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT0_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
        { /* BUT1 */
            &CapSense_ioList[8u],
            (void *)&CapSense_dsRam.wdgtList.but1,
            CapSense_dsRam.snsList.but1,
            (void *)CapSense_fltrHistoryBUT1,
            CapSense_debounceBUT1,
            CapSense_BUT1_STATIC_CONFIG,
            CapSense_BUT1_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT1_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
        { /* BUT3 */
            &CapSense_ioList[9u],
            (void *)&CapSense_dsRam.wdgtList.but3,
            CapSense_dsRam.snsList.but3,
            (void *)CapSense_fltrHistoryBUT3,
            CapSense_debounceBUT3,
            CapSense_BUT3_STATIC_CONFIG,
            CapSense_BUT3_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT3_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
        { /* BUT4 */
            &CapSense_ioList[10u],
            (void *)&CapSense_dsRam.wdgtList.but4,
            CapSense_dsRam.snsList.but4,
            (void *)CapSense_fltrHistoryBUT4,
            CapSense_debounceBUT4,
            CapSense_BUT4_STATIC_CONFIG,
            CapSense_BUT4_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT4_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
        { /* BUT5 */
            &CapSense_ioList[11u],
            (void *)&CapSense_dsRam.wdgtList.but5,
            CapSense_dsRam.snsList.but5,
            (void *)CapSense_fltrHistoryBUT5,
            CapSense_debounceBUT5,
            CapSense_BUT5_STATIC_CONFIG,
            CapSense_BUT5_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT5_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
        { /* BUT6 */
            &CapSense_ioList[12u],
            (void *)&CapSense_dsRam.wdgtList.but6,
            CapSense_dsRam.snsList.but6,
            (void *)CapSense_fltrHistoryBUT6,
            CapSense_debounceBUT6,
            CapSense_BUT6_STATIC_CONFIG,
            CapSense_BUT6_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT6_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
        { /* BUT8 */
            &CapSense_ioList[13u],
            (void *)&CapSense_dsRam.wdgtList.but8,
            CapSense_dsRam.snsList.but8,
            (void *)CapSense_fltrHistoryBUT8,
            CapSense_debounceBUT8,
            CapSense_BUT8_STATIC_CONFIG,
            CapSense_BUT8_NUM_SENSORS,
            (uint8)CapSense_WD_BUTTON_E,
            CapSense_BUT8_NUM_SENSORS,
            0u,
            0u,
            (void *)0u,
        },
    },
};


/**
* Array of pointers to the electrode specific register
*/
const CapSense_FLASH_IO_STRUCT CapSense_ioList[CapSense_TOTAL_ELECTRODES] =
{
    { /* 0: LinearSlider0_Sns0 */
        (reg32 *)CapSense_Sns__0__HSIOM,
        (reg32 *)CapSense_Sns__0__PC,
        (reg32 *)CapSense_Sns__0__DR,
        (reg32 *)CapSense_Sns__0__PS,
        CapSense_Sns__0__HSIOM_MASK,
        CapSense_Sns__0__MASK,
        CapSense_Sns__0__HSIOM_SHIFT,
        (uint8)CapSense_Sns__0__SHIFT,
        (uint8)CapSense_Sns__0__SHIFT * 3u,
    },
    { /* 1: LinearSlider0_Sns1 */
        (reg32 *)CapSense_Sns__1__HSIOM,
        (reg32 *)CapSense_Sns__1__PC,
        (reg32 *)CapSense_Sns__1__DR,
        (reg32 *)CapSense_Sns__1__PS,
        CapSense_Sns__1__HSIOM_MASK,
        CapSense_Sns__1__MASK,
        CapSense_Sns__1__HSIOM_SHIFT,
        (uint8)CapSense_Sns__1__SHIFT,
        (uint8)CapSense_Sns__1__SHIFT * 3u,
    },
    { /* 2: LinearSlider0_Sns2 */
        (reg32 *)CapSense_Sns__2__HSIOM,
        (reg32 *)CapSense_Sns__2__PC,
        (reg32 *)CapSense_Sns__2__DR,
        (reg32 *)CapSense_Sns__2__PS,
        CapSense_Sns__2__HSIOM_MASK,
        CapSense_Sns__2__MASK,
        CapSense_Sns__2__HSIOM_SHIFT,
        (uint8)CapSense_Sns__2__SHIFT,
        (uint8)CapSense_Sns__2__SHIFT * 3u,
    },
    { /* 3: LinearSlider0_Sns3 */
        (reg32 *)CapSense_Sns__3__HSIOM,
        (reg32 *)CapSense_Sns__3__PC,
        (reg32 *)CapSense_Sns__3__DR,
        (reg32 *)CapSense_Sns__3__PS,
        CapSense_Sns__3__HSIOM_MASK,
        CapSense_Sns__3__MASK,
        CapSense_Sns__3__HSIOM_SHIFT,
        (uint8)CapSense_Sns__3__SHIFT,
        (uint8)CapSense_Sns__3__SHIFT * 3u,
    },
    { /* 4: LinearSlider0_Sns4 */
        (reg32 *)CapSense_Sns__4__HSIOM,
        (reg32 *)CapSense_Sns__4__PC,
        (reg32 *)CapSense_Sns__4__DR,
        (reg32 *)CapSense_Sns__4__PS,
        CapSense_Sns__4__HSIOM_MASK,
        CapSense_Sns__4__MASK,
        CapSense_Sns__4__HSIOM_SHIFT,
        (uint8)CapSense_Sns__4__SHIFT,
        (uint8)CapSense_Sns__4__SHIFT * 3u,
    },
    { /* 5: LinearSlider0_Sns5 */
        (reg32 *)CapSense_Sns__5__HSIOM,
        (reg32 *)CapSense_Sns__5__PC,
        (reg32 *)CapSense_Sns__5__DR,
        (reg32 *)CapSense_Sns__5__PS,
        CapSense_Sns__5__HSIOM_MASK,
        CapSense_Sns__5__MASK,
        CapSense_Sns__5__HSIOM_SHIFT,
        (uint8)CapSense_Sns__5__SHIFT,
        (uint8)CapSense_Sns__5__SHIFT * 3u,
    },
    { /* 6: LinearSlider0_Sns6 */
        (reg32 *)CapSense_Sns__6__HSIOM,
        (reg32 *)CapSense_Sns__6__PC,
        (reg32 *)CapSense_Sns__6__DR,
        (reg32 *)CapSense_Sns__6__PS,
        CapSense_Sns__6__HSIOM_MASK,
        CapSense_Sns__6__MASK,
        CapSense_Sns__6__HSIOM_SHIFT,
        (uint8)CapSense_Sns__6__SHIFT,
        (uint8)CapSense_Sns__6__SHIFT * 3u,
    },
    { /* 7: BUT0_Sns0 */
        (reg32 *)CapSense_Sns__7__HSIOM,
        (reg32 *)CapSense_Sns__7__PC,
        (reg32 *)CapSense_Sns__7__DR,
        (reg32 *)CapSense_Sns__7__PS,
        CapSense_Sns__7__HSIOM_MASK,
        CapSense_Sns__7__MASK,
        CapSense_Sns__7__HSIOM_SHIFT,
        (uint8)CapSense_Sns__7__SHIFT,
        (uint8)CapSense_Sns__7__SHIFT * 3u,
    },
    { /* 8: BUT1_Sns0 */
        (reg32 *)CapSense_Sns__8__HSIOM,
        (reg32 *)CapSense_Sns__8__PC,
        (reg32 *)CapSense_Sns__8__DR,
        (reg32 *)CapSense_Sns__8__PS,
        CapSense_Sns__8__HSIOM_MASK,
        CapSense_Sns__8__MASK,
        CapSense_Sns__8__HSIOM_SHIFT,
        (uint8)CapSense_Sns__8__SHIFT,
        (uint8)CapSense_Sns__8__SHIFT * 3u,
    },
    { /* 9: BUT3_Sns0 */
        (reg32 *)CapSense_Sns__9__HSIOM,
        (reg32 *)CapSense_Sns__9__PC,
        (reg32 *)CapSense_Sns__9__DR,
        (reg32 *)CapSense_Sns__9__PS,
        CapSense_Sns__9__HSIOM_MASK,
        CapSense_Sns__9__MASK,
        CapSense_Sns__9__HSIOM_SHIFT,
        (uint8)CapSense_Sns__9__SHIFT,
        (uint8)CapSense_Sns__9__SHIFT * 3u,
    },
    { /* 10: BUT4_Sns0 */
        (reg32 *)CapSense_Sns__10__HSIOM,
        (reg32 *)CapSense_Sns__10__PC,
        (reg32 *)CapSense_Sns__10__DR,
        (reg32 *)CapSense_Sns__10__PS,
        CapSense_Sns__10__HSIOM_MASK,
        CapSense_Sns__10__MASK,
        CapSense_Sns__10__HSIOM_SHIFT,
        (uint8)CapSense_Sns__10__SHIFT,
        (uint8)CapSense_Sns__10__SHIFT * 3u,
    },
    { /* 11: BUT5_Sns0 */
        (reg32 *)CapSense_Sns__11__HSIOM,
        (reg32 *)CapSense_Sns__11__PC,
        (reg32 *)CapSense_Sns__11__DR,
        (reg32 *)CapSense_Sns__11__PS,
        CapSense_Sns__11__HSIOM_MASK,
        CapSense_Sns__11__MASK,
        CapSense_Sns__11__HSIOM_SHIFT,
        (uint8)CapSense_Sns__11__SHIFT,
        (uint8)CapSense_Sns__11__SHIFT * 3u,
    },
    { /* 12: BUT6_Sns0 */
        (reg32 *)CapSense_Sns__12__HSIOM,
        (reg32 *)CapSense_Sns__12__PC,
        (reg32 *)CapSense_Sns__12__DR,
        (reg32 *)CapSense_Sns__12__PS,
        CapSense_Sns__12__HSIOM_MASK,
        CapSense_Sns__12__MASK,
        CapSense_Sns__12__HSIOM_SHIFT,
        (uint8)CapSense_Sns__12__SHIFT,
        (uint8)CapSense_Sns__12__SHIFT * 3u,
    },
    { /* 13: BUT8_Sns0 */
        (reg32 *)CapSense_Sns__13__HSIOM,
        (reg32 *)CapSense_Sns__13__PC,
        (reg32 *)CapSense_Sns__13__DR,
        (reg32 *)CapSense_Sns__13__PS,
        CapSense_Sns__13__HSIOM_MASK,
        CapSense_Sns__13__MASK,
        CapSense_Sns__13__HSIOM_SHIFT,
        (uint8)CapSense_Sns__13__SHIFT,
        (uint8)CapSense_Sns__13__SHIFT * 3u,
    },
};



/** \}
* \endif */

/* Initialization data for RAM widget list */
static const CapSense_RAM_WD_LIST_STRUCT CapSense_ramWidgetInit =
{
    { /* LinearSlider0 */
        CapSense_LINEARSLIDER0_RESOLUTION,
        CapSense_LINEARSLIDER0_FINGER_TH,
        CapSense_LINEARSLIDER0_NOISE_TH,
        CapSense_LINEARSLIDER0_NNOISE_TH,
        CapSense_LINEARSLIDER0_HYSTERESIS,
        CapSense_LINEARSLIDER0_ON_DEBOUNCE,
        CapSense_LINEARSLIDER0_LOW_BSLN_RST,
        CapSense_LINEARSLIDER0_BSLN_COEFF,
        {
            CapSense_LINEARSLIDER0_IDAC_MOD0,
        },
        CapSense_LINEARSLIDER0_SNS_CLK,
        CapSense_LINEARSLIDER0_SNS_CLK_SOURCE,
        CapSense_LINEARSLIDER0_FINGER_CAP,
        CapSense_LINEARSLIDER0_SIGPFC,
        {
            CapSense_LINEARSLIDER0_POSITION,
        },
    },
    { /* BUT0 */
        CapSense_BUT0_RESOLUTION,
        CapSense_BUT0_FINGER_TH,
        CapSense_BUT0_NOISE_TH,
        CapSense_BUT0_NNOISE_TH,
        CapSense_BUT0_HYSTERESIS,
        CapSense_BUT0_ON_DEBOUNCE,
        CapSense_BUT0_LOW_BSLN_RST,
        CapSense_BUT0_BSLN_COEFF,
        {
            CapSense_BUT0_IDAC_MOD0,
        },
        CapSense_BUT0_SNS_CLK,
        CapSense_BUT0_SNS_CLK_SOURCE,
        CapSense_BUT0_FINGER_CAP,
        CapSense_BUT0_SIGPFC,
    },
    { /* BUT1 */
        CapSense_BUT1_RESOLUTION,
        CapSense_BUT1_FINGER_TH,
        CapSense_BUT1_NOISE_TH,
        CapSense_BUT1_NNOISE_TH,
        CapSense_BUT1_HYSTERESIS,
        CapSense_BUT1_ON_DEBOUNCE,
        CapSense_BUT1_LOW_BSLN_RST,
        CapSense_BUT1_BSLN_COEFF,
        {
            CapSense_BUT1_IDAC_MOD0,
        },
        CapSense_BUT1_SNS_CLK,
        CapSense_BUT1_SNS_CLK_SOURCE,
        CapSense_BUT1_FINGER_CAP,
        CapSense_BUT1_SIGPFC,
    },
    { /* BUT3 */
        CapSense_BUT3_RESOLUTION,
        CapSense_BUT3_FINGER_TH,
        CapSense_BUT3_NOISE_TH,
        CapSense_BUT3_NNOISE_TH,
        CapSense_BUT3_HYSTERESIS,
        CapSense_BUT3_ON_DEBOUNCE,
        CapSense_BUT3_LOW_BSLN_RST,
        CapSense_BUT3_BSLN_COEFF,
        {
            CapSense_BUT3_IDAC_MOD0,
        },
        CapSense_BUT3_SNS_CLK,
        CapSense_BUT3_SNS_CLK_SOURCE,
        CapSense_BUT3_FINGER_CAP,
        CapSense_BUT3_SIGPFC,
    },
    { /* BUT4 */
        CapSense_BUT4_RESOLUTION,
        CapSense_BUT4_FINGER_TH,
        CapSense_BUT4_NOISE_TH,
        CapSense_BUT4_NNOISE_TH,
        CapSense_BUT4_HYSTERESIS,
        CapSense_BUT4_ON_DEBOUNCE,
        CapSense_BUT4_LOW_BSLN_RST,
        CapSense_BUT4_BSLN_COEFF,
        {
            CapSense_BUT4_IDAC_MOD0,
        },
        CapSense_BUT4_SNS_CLK,
        CapSense_BUT4_SNS_CLK_SOURCE,
        CapSense_BUT4_FINGER_CAP,
        CapSense_BUT4_SIGPFC,
    },
    { /* BUT5 */
        CapSense_BUT5_RESOLUTION,
        CapSense_BUT5_FINGER_TH,
        CapSense_BUT5_NOISE_TH,
        CapSense_BUT5_NNOISE_TH,
        CapSense_BUT5_HYSTERESIS,
        CapSense_BUT5_ON_DEBOUNCE,
        CapSense_BUT5_LOW_BSLN_RST,
        CapSense_BUT5_BSLN_COEFF,
        {
            CapSense_BUT5_IDAC_MOD0,
        },
        CapSense_BUT5_SNS_CLK,
        CapSense_BUT5_SNS_CLK_SOURCE,
        CapSense_BUT5_FINGER_CAP,
        CapSense_BUT5_SIGPFC,
    },
    { /* BUT6 */
        CapSense_BUT6_RESOLUTION,
        CapSense_BUT6_FINGER_TH,
        CapSense_BUT6_NOISE_TH,
        CapSense_BUT6_NNOISE_TH,
        CapSense_BUT6_HYSTERESIS,
        CapSense_BUT6_ON_DEBOUNCE,
        CapSense_BUT6_LOW_BSLN_RST,
        CapSense_BUT6_BSLN_COEFF,
        {
            CapSense_BUT6_IDAC_MOD0,
        },
        CapSense_BUT6_SNS_CLK,
        CapSense_BUT6_SNS_CLK_SOURCE,
        CapSense_BUT6_FINGER_CAP,
        CapSense_BUT6_SIGPFC,
    },
    { /* BUT8 */
        CapSense_BUT8_RESOLUTION,
        CapSense_BUT8_FINGER_TH,
        CapSense_BUT8_NOISE_TH,
        CapSense_BUT8_NNOISE_TH,
        CapSense_BUT8_HYSTERESIS,
        CapSense_BUT8_ON_DEBOUNCE,
        CapSense_BUT8_LOW_BSLN_RST,
        CapSense_BUT8_BSLN_COEFF,
        {
            CapSense_BUT8_IDAC_MOD0,
        },
        CapSense_BUT8_SNS_CLK,
        CapSense_BUT8_SNS_CLK_SOURCE,
        CapSense_BUT8_FINGER_CAP,
        CapSense_BUT8_SIGPFC,
    },
};


/* IDAC Initialization Data */
static const uint8 CapSense_ramIdacInit[CapSense_TOTAL_SENSORS] =
{
    /* LinearSlider0 */
    CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0,
    CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0,
    CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0,
    CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0,
    CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0,
    CapSense_LINEARSLIDER0_SNS5_IDAC_COMP0,
    CapSense_LINEARSLIDER0_SNS6_IDAC_COMP0,

    /* BUT0 */
    CapSense_BUT0_SNS0_IDAC_COMP0,

    /* BUT1 */
    CapSense_BUT1_SNS0_IDAC_COMP0,

    /* BUT3 */
    CapSense_BUT3_SNS0_IDAC_COMP0,

    /* BUT4 */
    CapSense_BUT4_SNS0_IDAC_COMP0,

    /* BUT5 */
    CapSense_BUT5_SNS0_IDAC_COMP0,

    /* BUT6 */
    CapSense_BUT6_SNS0_IDAC_COMP0,

    /* BUT8 */
    CapSense_BUT8_SNS0_IDAC_COMP0,
};


/*******************************************************************************
* Define internal data types and statements
*******************************************************************************/
#define PARAM_TYPE_OFFSET       (30u)
#define PARAM_TYPE_MASK         (3Lu << PARAM_TYPE_OFFSET)
#define PARAM_RWBIT_OFFSET      (29u)
#define PARAM_RWBIT_MASK        (1Lu << PARAM_RWBIT_OFFSET)
#define PARAM_FBIT_OFFSET       (28u)
#define PARAM_FBIT_MASK         (1Lu << PARAM_FBIT_OFFSET)
#define PARAM_UBIT_OFFSET       (23u)
#define PARAM_UBIT_MASK         (1Lu << PARAM_UBIT_OFFSET)
#define PARAM_WDID_OFFSET       (16u)
#define PARAM_WDID_MASK         (0x7FLu << PARAM_WDID_OFFSET)
#define PARAM_CRC_OFFSET        (24u)
#define PARAM_CRC_MASK          (0x0FLu << PARAM_CRC_OFFSET)

/* Define align masks for uint16 and uint32 */
#define UINT16_ALIGN_MASK       (1Lu)
#define UINT32_ALIGN_MASK       (3Lu)

/* Define supported parameter types */
#define DS_PARAM_TYPE_UINT8     (0x01u)
#define DS_PARAM_TYPE_UINT16    (0x02u)
#define DS_PARAM_TYPE_UINT32    (0x03u)


/* Define PARAM_ID structure */
typedef struct
{
    uint16 offset;      /* parameter offset                     */
    uint8  widgetId;    /* widget Id parameter belongs to       */
    uint8  affectsCrc;  /* parameter affects widget CRC         */
    uint8  crc;         /* parameter CRC                        */
    uint8  flash;       /* parameter located in FLASH flag      */
    uint8  rw;          /* parameter is read/write flag         */
    uint8  type;        /* parameter type: uint8/uint16/uint32  */
} PARAM_ID_STRUCT;


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

static cystatus DsParseParamId(uint32 paramId, PARAM_ID_STRUCT *pData);

/** \}
* \endif */


/*******************************************************************************
* Function Name: CapSense_DsInitialize
****************************************************************************//**
*
* \brief
*   This function initializes the Data Structure storage.
*
* \details
*   This function copies the default widget configuration parameters 
*   from the Flash storage into the RAM Data Structure.
*
*******************************************************************************/
void CapSense_DsInitialize(void)
{
    #if (0u != CapSense_CSX_EN) || (0 != CapSense_CSD_IDAC_COMP_EN)
        uint32 snsId;
        uint32 wdgtId;
    
        CapSense_RAM_SNS_STRUCT * ptrSns;
        CapSense_FLASH_WD_STRUCT const * ptrFlashWdgt;
        uint8 const * ptrIdacInit = CapSense_ramIdacInit;
    #endif        
    
    /* Reset RAM data structure content */
    (void)memset(&CapSense_dsRam, 0, sizeof(CapSense_dsRam));
    
    /* Initialize configId and deviceId registers */
    CapSense_dsRam.configId = CapSense_CONFIG_ID;
    CapSense_dsRam.deviceId = CapSense_DEVICE_ID;
    
    /* Initialize global RAM data */
    CapSense_dsRam.csd0Config = CapSense_CSD0_CONFIG;

    #if ((0u != CapSense_CSD2X_EN) || (0u != CapSense_CSX2X_EN))
        CapSense_dsRam.csd1Config = CapSense_CSD1_CONFIG;
    #endif /* #if ((0u != CapSense_CSD2X_EN) || (0u != CapSense_CSX2X_EN)) */
    
    #if (0u != CapSense_TOTAL_CSD_WIDGETS)
        CapSense_dsRam.modCsdClk = CapSense_CSD_SCANSPEED_DIVIDER;
        
        #if (0u != CapSense_CSD_COMMON_SNS_CLK_EN)
            CapSense_dsRam.snsCsdClk = CapSense_CSD_SNS_CLK_DIVIDER;
        #endif /* #if (0u != CapSense_CSD_COMMON_SNS_CLK_EN) */
    #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */
    
    #if (0u != CapSense_CSX_EN)
        CapSense_dsRam.modCsxClk = CapSense_CSX_SCANSPEED_DIVIDER;
        
        #if (0u != CapSense_CSX_COMMON_TX_CLK_EN)
            CapSense_dsRam.snsCsxClk = CapSense_CSX_TX_CLK_DIVIDER;
        #endif /* #if (0u != CapSense_CSX_COMMON_TX_CLK_EN) */
    #endif /* #if (0u != CapSense_CSX_EN) */
    
    #if (0u != CapSense_SELF_TEST_EN)
        CapSense_dsRam.glbCrc = CapSense_GLB_CRC;
    #endif /* #if (0u != CapSense_SELF_TEST_EN) */
   
    /* Initialize RAM widget data */
    CapSense_dsRam.wdgtList = CapSense_ramWidgetInit;
    
    #if (0u != CapSense_CSX_EN) || (0 != CapSense_CSD_IDAC_COMP_EN)
        /* Initialize IDAC data */
        ptrFlashWdgt = CapSense_dsFlash.wdgtArray;
        
        for (wdgtId = CapSense_TOTAL_WIDGETS; wdgtId-- > 0u; )
        {
            ptrSns = ptrFlashWdgt->ptr2SnsRam;
                
            for (snsId = CapSense_GET_SNS_CNT_BY_PTR(ptrFlashWdgt); snsId-- > 0u;)
            {
                ptrSns->idacComp[0u] = *ptrIdacInit;
                ptrIdacInit++;
                
                #if (0u != CapSense_MULTI_FREQ_SCAN_EN)
                    ptrSns->idacComp[1u] = *ptrIdacInit;
                    ptrIdacInit++;
                    
                    ptrSns->idacComp[2u] = *ptrIdacInit;
                    ptrIdacInit++;
                #endif /* #if (0u != CapSense_MULTI_FREQ_SCAN_EN) */
                
                ptrSns++;
            }
            
            ptrFlashWdgt++; /* Move to next widget */
        }
    #endif /*(0u != CapSense_CSX_EN) || (0 != CapSense_CSD_IDAC_COMP_EN)*/
    
    #if (0u != CapSense_ADC_EN)
        CapSense_AdcDsInitialize();
    #endif /* (0u != CapSense_ADC_EN) */
}


#if (0u != CapSense_ADC_EN)
    /*******************************************************************************
    * Function Name: CapSense_AdcDsInitialize
    ****************************************************************************//**
    *
    * \brief
    *   Configures the initial Adc datastructure members.
    *
    *******************************************************************************/
    void CapSense_AdcDsInitialize(void)
    {
        CapSense_dsRam.adcResolution = CapSense_ADC_RESOLUTION;
        CapSense_dsRam.adcIdac = (uint8)(CapSense_ADC_IDACVAL);
    }
#endif /* (0u != CapSense_ADC_EN) */


/*******************************************************************************
* Function Name: DsParseParamId
****************************************************************************//**
*
* \brief
*   This function parses the parameter ID and checks its correctness.
*
* \details
*   This function checks the parameter CRC if the self-test is enabled, 
*   validates the offset value and parameter type.
*
* \param paramId The parameter to parse.
* \param pData   The pointer to the structure that will hold parsed data.
*
* \return CYRET_SUCCESS     If the parameter is valid
*         CYRET_BAD_PARAM   If  the parameter is invalid
*
*******************************************************************************/
static cystatus DsParseParamId(uint32 paramId, PARAM_ID_STRUCT *pData)
{
    cystatus result = CYRET_SUCCESS;
    
    #if (0u != CapSense_SELF_TEST_EN)
        const uint8 crcTable[] =
        {
            0x00u, 0x09u, 0x0Bu, 0x02u, 0x0Fu, 0x06u, 0x04u, 0x0Du,
            0x07u, 0x0Eu, 0x0Cu, 0x05u, 0x08u, 0x01u, 0x03u, 0x0Au
        };
        
        uint32 i;
        uint32 actualCrc = 0u;
        uint32 crcIndex;
    #endif /* #if (0u != CapSense_SELF_TEST_EN) */
            
    /* Extract parameter data */
    pData->offset     = LO16(paramId);
    pData->affectsCrc = LO8((paramId & PARAM_UBIT_MASK)  >> PARAM_UBIT_OFFSET);
    pData->widgetId   = LO8((paramId & PARAM_WDID_MASK)  >> PARAM_WDID_OFFSET);
    pData->type       = LO8((paramId & PARAM_TYPE_MASK)  >> PARAM_TYPE_OFFSET);
    pData->flash      = LO8((paramId & PARAM_FBIT_MASK)  >> PARAM_FBIT_OFFSET);
    pData->rw         = LO8((paramId & PARAM_RWBIT_MASK) >> PARAM_RWBIT_OFFSET);
    pData->crc        = LO8((paramId & PARAM_CRC_MASK)   >> PARAM_CRC_OFFSET);
    
    /* Check parameter CRC if self-test is enabled */
    #if (0u != CapSense_SELF_TEST_EN)
 
        /* Calculate CRC for bits 0..24.
         * The CRC is calculated using nibbles (4-bits). 
         * So for 0..24 bits there are in total 6 nibbles. */
        for (i = 6u; i > 0u; i--)
        {
            crcIndex = actualCrc ^ (paramId & 0x0Fu);
            actualCrc = crcTable[crcIndex];
            paramId >>= 4u;
        }

        /* Add bits 28..32 */
        crcIndex = actualCrc ^ ((paramId >> 4u) & 0x0Fu);
        actualCrc = crcTable[crcIndex];

        /* Add bits 24..28 (CRC) */
        crcIndex = actualCrc ^ (paramId & 0x0Fu);
        actualCrc = crcTable[crcIndex];
        
        if (0u != actualCrc)
        {
            /* CRC mismatch */
            result = CYRET_BAD_PARAM;
        }
        
        if ((CYRET_SUCCESS == result) && 
            (0u != pData->affectsCrc) && (pData->widgetId >= CapSense_TOTAL_WIDGETS ))
        {
            /* Wrong widgetId for protected parameter */
            result = CYRET_BAD_PARAM;
        }
        
        /* Check offset value range */
        if (CYRET_SUCCESS == result)
        {
            if (0u == pData->flash)
            {
                /* Check offset for RAM Data Structure range */
                if (pData->offset >= sizeof(CapSense_dsRam))
                {
                    result = CYRET_BAD_PARAM;
                }
            }
            else
            {
                /* Check offset for Flash Data Structure range */
                if (pData->offset >= sizeof(CapSense_dsFlash))
                {
                    result = CYRET_BAD_PARAM;
                }
                
                /* Check if it is marked as Read Only */
                if (0u != pData->rw)
                {
                    result = CYRET_BAD_PARAM;
                }
            }
        }
    #else /* #if (0u != CapSense_SELF_TEST_EN) */
        /* Check offset value range */
        if (0u == pData->flash)
        {
            /* Check offset for RAM Data Structure range */
            if (pData->offset >= sizeof(CapSense_dsRam))
            {
                result = CYRET_BAD_PARAM;
            }
        }
        else
        {
            /* Check offset for Flash Data Structure range */
            if (pData->offset >= sizeof(CapSense_dsFlash))
            {
                result = CYRET_BAD_PARAM;
            }
            
            /* Check if it is marked as Read Only */
            if (0u != pData->rw)
            {
                result = CYRET_BAD_PARAM;
            }
        }
    #endif /* #if (0u != CapSense_SELF_TEST_EN) */

    if (CYRET_SUCCESS == result)
    {
        /* Check parameter type, offset alignment */
        switch (pData->type)
        {
        case DS_PARAM_TYPE_UINT16:
            if (0u != (pData->offset & UINT16_ALIGN_MASK))
            {
                result = CYRET_BAD_PARAM;
            }
            break;
            
        case DS_PARAM_TYPE_UINT32:
            if (0u != (pData->offset & UINT32_ALIGN_MASK))
            {
                result = CYRET_BAD_PARAM;
            }
            break;
            
        case DS_PARAM_TYPE_UINT8:
            break;
            
        default:
            result = CYRET_BAD_PARAM;
            break;
        }
    }

    return result;
}

/*******************************************************************************
* Function Name: CapSense_GetParam
****************************************************************************//**
*
* \brief
*  Gets the specified parameter value from the \ref group_structures.
*
* \details
*  This function gets the value of the specified parameter by the paramId 
*  argument. The paramId for each register is available in the 
*  CapSense RegisterMap header file as 
*  CapSense_<ParameterName>_PARAM_ID. The paramId is a special 
*  enumerated value generated by the customizer. The format of paramId is as 
*  follows:
*    1. [ byte 3 byte 2 byte 1 byte 0 ] 
*    2. [ TTWFCCCC UIIIIIII MMMMMMMM LLLLLLLL ]
*    3. T - encodes the parameter type:
*      - 01b: uint8
*      - 10b: uint16
*      - 11b: uint32
*    4. W - indicates whether the parameter is writable:
*      - 0: ReadOnly
*      - 1: Read/Write
*    5. C - 4 bit CRC (X^3 + 1) of the whole paramId word, the C bits are 
*       filled with 0s when the CRC is calculated.
*    6. U - indicates if the parameter affects the RAM Widget Object CRC.
*    7. I - specifies that the widgetID parameter belongs to
*    8. M,L - the parameter offset MSB and LSB accordingly in:
*      - Flash Data Structure if W bit is 0.
*      - RAM Data Structure if W bit is 1.
*
*  Refer to the \ref group_structures section for details of the data structure 
*  organization and examples of its register access.
*
* \param  paramId 
*  Specify the ID of parameter to get its value.
*  A macro for the parameter ID can be found in the CapSense RegisterMap header 
*  file defined as CapSense_<ParameterName>_PARAM_ID.
*
* \param  value   
*  The pointer to a variable that should be updated with the got value.
*
* \return
*  Returns the status of operation:
*    - CYRET_SUCCESS if the operation is successfully completed.
*    - CYRET_BAD_PARAM if the input parameter is invalid. 
*
*******************************************************************************/
cystatus CapSense_GetParam(uint32 paramId, uint32 *value)
{
    PARAM_ID_STRUCT pData;
    cystatus result;
    
    union
    {
        volatile void   const * ptr;
        volatile uint8  const * ptrUint8;
        volatile uint16 const * ptrUint16;
        volatile uint32 const * ptrUint32;
    } ptrData;
    
    result = DsParseParamId(paramId, &pData);
    
    /* Parse and validate paramId */
    if (CYRET_SUCCESS == result)
    {
        /* Get base address of parameter */
        if (0u == pData.flash)
        {
            ptrData.ptr = &CapSense_dsRam;
        }
        else
        {
            ptrData.ptr = &CapSense_dsFlash;
        }
        
        /* Add offset to base address */
        ptrData.ptrUint8 += pData.offset;
        
        /* Read data */
        if (DS_PARAM_TYPE_UINT8 == pData.type)
        {
            *value = (uint32) (*ptrData.ptrUint8);
        }
        else if (DS_PARAM_TYPE_UINT16 == pData.type)
        {
            *value = (uint32) (*ptrData.ptrUint16);
        }
        else if (DS_PARAM_TYPE_UINT32 == pData.type)
        {
            *value = *ptrData.ptrUint32;
        }
        else
        {
            /* Parameter comes here already validated. */
            CYASSERT(0u);
        }
    }
    
    return result;
}


/*******************************************************************************
* Function Name: CapSense_SetParam
****************************************************************************//**
*
* \brief
*  Sets a new value for the specified parameter in the \ref group_structures.
*
* \details
*  This function sets the value of the specified parameter by the paramId 
*  argument. The paramId for each register is available in the 
*  CapSense RegisterMap header file as 
*  CapSense_<ParameterName>_PARAM_ID. The paramId is a special 
*  enumerated value generated by the customizer. The format of paramId is as 
*  follows:
*    1. [ byte 3 byte 2 byte 1 byte 0 ] 
*    2. [ TTWFCCCC UIIIIIII MMMMMMMM LLLLLLLL ]
*    3. T - encodes the parameter type:
*      - 01b: uint8
*      - 10b: uint16
*      - 11b: uint32
*    4. W - indicates whether the parameter is writable:
*      - 0: ReadOnly
*      - 1: Read/Write
*    5. C - 4 bit CRC (X^3 + 1) of the whole paramId word, the C bits are 
*       filled with 0s when the CRC is calculated.
*    6. U - indicates if the parameter affects the RAM Widget Object CRC.
*    7. I - specifies that the widgetID parameter belongs to
*    8. M,L - the parameter offset MSB and LSB accordingly in:
*      - Flash Data Structure if W bit is 0.
*      - RAM Data Structure if W bit is 1.
*
*  Refer to the \ref group_structures section for details of the data structure 
*  organization and examples of its register access.
*
* \param paramId  
*  Specify the ID of parameter to set its value.
*  A macro for the parameter ID can be found in the CapSense RegisterMap header
*  file defined as CapSense_<ParameterName>_PARAM_ID.

* \param value    
*  Specify the new parameter's value.
*
* \return
*  Returns the status of operation:
*    - CYRET_SUCCESS if the operation is successfully completed.
*    - CYRET_BAD_PARAM if the input parameter is invalid. 
*
*******************************************************************************/
cystatus CapSense_SetParam(uint32 paramId, uint32 value)
{
    cystatus result;
    PARAM_ID_STRUCT pData;
    
    union
    {
        volatile void   * ptr;
        volatile uint8  * ptrUint8;
        volatile uint16 * ptrUint16;
        volatile uint32 * ptrUint32;
    } ptrData;
    
    result = DsParseParamId(paramId, &pData);
    
    /* verify if parameter is writable */
    if (CYRET_SUCCESS == result)
    {
        if (0u == pData.rw)
        {
            result = CYRET_BAD_PARAM;
        }
    }
    
    /* Parse and validate paramId */
    if (CYRET_SUCCESS == result)
    {
        /* Get base address of parameter */
        ptrData.ptr = &CapSense_dsRam;
        
        /* Add offset to base address */
        ptrData.ptrUint8 += pData.offset;
        
        /* Write data */
        if (DS_PARAM_TYPE_UINT8 == pData.type)
        {
            *ptrData.ptrUint8 = LO8(value);
        }
        else if (DS_PARAM_TYPE_UINT16 == pData.type)
        {
            *ptrData.ptrUint16 = LO16(value);
        }
        else if (DS_PARAM_TYPE_UINT32 == pData.type)
        {
            *ptrData.ptrUint32 = value;
        }
        else
        {
            /* Parameter comes here already validated. */
            CYASSERT(0u);
        }
        
        #if (CapSense_ENABLE == (CapSense_TST_WDGT_CRC_EN || CapSense_TST_GLOBAL_CRC_EN))
            /* Update widget or global CRC if self-test is enabled and parameter affects it */
            if (0u != pData.affectsCrc)
            {
                if ((ptrData.ptrUint16 >= &CapSense_dsRam.csd0Config) &&
                    (ptrData.ptrUint16 <  &CapSense_dsRam.glbCrc))
                {
                    
                    #if (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN)
                        CapSense_DsUpdateGlobalCrc();
                    #endif /* (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN) */

                }
                else
                {
                    #if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
                        CapSense_DsUpdateWidgetCrc((uint32)pData.widgetId);
                    #endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */
                }
            }
        #endif /* (CapSense_ENABLE == (CapSense_TST_WDGT_CRC_EN || CapSense_TST_GLOBAL_CRC_EN)) */
    }
    
    return result;
}

/*******************************************************************************
* Function Name: CapSense_IsAnyWidgetActive
****************************************************************************//**
*
* \brief
*  Reports if any widget has detected a touch.
*
* \details
*  This function reports if any widget has detected a touch or not by extracting 
*  the information from the wdgtStatus registers 
*  (CapSense_WDGT_STATUS<X>_VALUE). This function does not process any 
*  widget but extracts the processed results from the \ref group_structures.
*
* \return 
*   Returns the touch detection status of all widgets:
*     - Zero if no touch is detected in all widgets or sensors.
*     - Non-zero if at least one widget or sensor detected a touch.
*
*******************************************************************************/
uint32 CapSense_IsAnyWidgetActive(void)
{
    uint32 result = 0Lu;
    uint32 wdWord;
    
    for (wdWord = CapSense_WDGT_STATUS_WORDS; wdWord-- > 0u;)
    {
        result |= CapSense_dsRam.wdgtStatus[wdWord];
    }
    
    return result;
}


/*******************************************************************************
* Function Name: CapSense_IsWidgetActive
****************************************************************************//**
*
* \brief
*  Reports if the specified widget detects a touch on any of its sensors.
*
* \details
*  This function reports if the specified widget has detected a touch or not by 
*  extracting the information from the wdgtStatus registers 
*  (CapSense_WDGT_STATUS<X>_VALUE). This function does not process the 
*  widget, but extracts the processed results from the \ref group_structures.
*
* \param widgetId  
*  Specify the ID number of the widget to get its status.
*  A macro for the widget ID can be found in the CapSense Configuration header 
*  file defined as CapSense_<WidgetName>_WDGT_ID.
*
* \return 
*  Returns the touch detection status of the specified widgets:
*    - Zero if no touch is detected in the specified widget or a wrong widgetId
*      is specified
*    - Non-zero if at least one sensor of the specified widget is active i.e. 
*      touch is detected
*
*******************************************************************************/
uint32 CapSense_IsWidgetActive(uint32 widgetId)
{
    uint32 result = 0Lu;
   
    if (widgetId < CapSense_TOTAL_WIDGETS)
    {
        result = CapSense_GET_WIDGET_ACTIVE_STATUS(widgetId);
    }
    return result;
}


/*******************************************************************************
* Function Name: CapSense_IsSensorActive
****************************************************************************//**
*
* \brief
*  Reports if the specified sensor in the widget detects a touch.
*
* \details 
*  This function reports if the specified sensor in the widget has detected a 
*  touch or not by extracting the information from wdgtStatus registers 
*  (CapSense_WDGT_STATUS<X>_VALUE). This function does not process the
*  widget or sensor, but extracts the processed results from the \ref group_structures.
*   
*  For Proximity sensors this function returns proximity detection status. To 
*  get finger touch status of proximity sensors, use the 
*  CapSense_IsProximitySensorActive() function. 
*
* \param widgetId  
*  Specify the ID number of the widget.
*  A macro for the widget ID can be found in the CapSense Configuration header 
*  file defined as CapSense_<WidgetName>_WDGT_ID 

* \param sensorId  
*  Specify the ID number of the sensor within the widget to get its touch 
*  detection status.
*  A macro for the sensor ID within specified widget can be found in the 
*  CapSense Configuration header file defined as 
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID 
*
* \return 
*  Returns the touch detection status of the specified sensor / widget:
*    - Zero if no touch is detected in the specified sensor / widget or a wrong
*      widget ID / sensor ID is specified
*    - Non-zero if the specified sensor is active i.e. touch is detected. If the
*      specific sensor belongs to a proximity widget, the proximity detection 
*      status is returned.
*
*******************************************************************************/
uint32 CapSense_IsSensorActive(uint32 widgetId, uint32 sensorId)
{
    uint32 result = 0Lu;
    
    if ((widgetId < CapSense_TOTAL_WIDGETS) &&
        (sensorId < CapSense_GET_SENSOR_COUNT(widgetId)))
    {
        #if (CapSense_ENABLE == CapSense_PROXIMITY_WIDGET_EN)
            if (CapSense_WD_PROXIMITY_E == 
				(CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[widgetId].wdgtType)
            {
                sensorId = CapSense_PROX_STS_OFFSET(sensorId);
            }
        #endif /* (CapSense_ENABLE != CapSense_PROXIMITY_WIDGET_EN) */
        result = CapSense_dsRam.snsStatus[widgetId] & (1Lu << sensorId);
    }
    
    return result;
}

#if (0u != CapSense_MATRIX_WIDGET_EN)
/*******************************************************************************
* Function Name: CapSense_IsMatrixButtonsActive
****************************************************************************//**
*
* \brief
*  Reports the status of the specified matrix button widget.
*
* \details 
*  This function reports if the specified matrix widget has detected a touch or
*  not by extracting the information from the wdgtStatus registers 
*  (CapSense_WDGT_STATUS<X>_VALUE for the CSD widgets and 
*  CapSense_SNS_STATUS<WidgetId>_VALUE for CSX widget). In addition, the
*  function provides details of the active sensor including active rows/columns 
*  for the CSD widgets. This function should be used only with the matrix button
*  widgets. This function does not process the widget, but extracts the 
*  processed results from the \ref group_structures.
*
* \param widgetId 
*  Specify the ID number of the matrix button widget to check status of its 
*  sensors. 
*  A macro for widget ID can be found in the CapSense Configuration header file 
*  defined as CapSense_<WidgetName>_WDGT_ID
*
* \return 
*  Returns the touch detection status of sensors in the specified matrix 
*  buttons widget. Zero indicates that no touch is detected in the specified 
*  widget or a wrong widgetId is specified.
*    1. For the matrix buttons widgets with the CSD sensing mode:
*      - Bit [31] if set, indicates that one or more sensors in the widget 
*        detected a touch.
*      - Bits [30..24] is reserved
*      - Bits [23..16] indicates the logical sensor number of the sensor that 
*        detected a touch. If more than one sensor detected a touch for the CSD
*        widget, no status is reported as more than one touch is invalid for the
*        CSD matrix buttons widgets.
*      - Bits [15..8] indicates the active row number.
*      - Bits [7..0] indicates the active column number.
*    2. For the matrix buttons widgets with the CSX widgets, each bit (31..0)
*      corresponds to the TX/RX intersection.
*
*******************************************************************************/
uint32 CapSense_IsMatrixButtonsActive(uint32 widgetId)
{
    uint32 result = 0Lu;
    cystatus state = CYRET_SUCCESS;
    CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt = 0u;
    
    #if (0u != CapSense_CSD_MATRIX_WIDGET_EN)
        CapSense_RAM_WD_CSD_MATRIX_STRUCT *wdCsdMatrix;
    #endif
  
    if (widgetId >= CapSense_TOTAL_WIDGETS)
    {
        state = CYRET_BAD_PARAM;
    }
    else
    {
        ptrFlashWdgt = &CapSense_dsFlash.wdgtArray[widgetId];
        
        if ((CapSense_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType != CapSense_WD_MATRIX_BUTTON_E)
        {
            state = CYRET_BAD_PARAM;
        }
        else if (0u == CapSense_GET_WIDGET_ACTIVE_STATUS(widgetId))
        {
            state = CYRET_BAD_PARAM;
        }
        else
        {
            /* input parameters are OK */
        }
    }
       
    if (CYRET_SUCCESS == state)
    {
        switch(CapSense_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapSense_CSD_MATRIX_WIDGET_EN)
            case CapSense_SENSE_METHOD_CSD_E:
                wdCsdMatrix = ptrFlashWdgt->ptr2WdgtRam;
                
                result = CapSense_MATRIX_BUTTONS_TOUCHED | 
                         ((uint32)wdCsdMatrix->posSnsId << 16u)  |
                         ((uint32)wdCsdMatrix->posRow   << 8u)   |
                         (uint32)wdCsdMatrix->posCol;
                break;
        #endif /* #if (0u != CapSense_CSD_MATRIX_WIDGET_EN) */
        
        #if (0u != CapSense_CSX_MATRIX_WIDGET_EN)
            case CapSense_SENSE_METHOD_CSX_E:
                result = CapSense_dsRam.snsStatus[widgetId];
                break;
        #endif /* #if (0u != CapSense_CSX_MATRIX_WIDGET_EN) */
        
        default:
            CYASSERT(0u);
            break;
        }
    }
    
    return result;
}
#endif /* #if (0u != CapSense_MATRIX_WIDGET_EN) */

#if (0u != CapSense_PROXIMITY_WIDGET_EN)
/*******************************************************************************
* Function Name: CapSense_IsProximitySensorActive
****************************************************************************//**
*
* \brief
*  Reports the finger detection status of the specified proximity widget/sensor.
*
* \details 
*  This function reports if the specified proximity sensor has detected a touch
*  or not by extracting the information from the wdgtStatus registers 
*  (CapSense_SNS_STATUS<WidgetId>_VALUE). This function should be used 
*  only with the proximity sensor widgets. This function does not process the 
*  widget but extracts the processed results from the \ref group_structures.
*
* \param widgetId  
*  Specify the ID number of the proximity widget.
*  A macro for the widget ID can be found in the CapSense Configuration header 
*  file defined as CapSense_<WidgetName>_WDGT_ID
*  
* \param proxId    
*  Specify the ID number of the proximity sensor within the proximity widget to 
*  get its touch detection status.
*  A macro for the proximity ID within a specified widget can be found in the
*  CapSense Configuration header file defined as 
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID
*
* \return 
*  Returns the status of the specified sensor of proximity widget. Zero 
*  indicates that no touch is detected in the specified sensor / widget or a 
*  wrong widgetId / proxId is specified.
*    - Bits [31..2] is reserved.
*    - Bit [1] indicates that a touch is detected.
*    - Bit [0] indicates that a proximity is detected.
*
*******************************************************************************/
uint32 CapSense_IsProximitySensorActive(uint32 widgetId, uint32 proxId)
{
    uint32 result = 0Lu;
    
    if ((widgetId < CapSense_TOTAL_WIDGETS) && (proxId < CapSense_GET_SENSOR_COUNT(widgetId)) &&
        (CapSense_WD_PROXIMITY_E == 
            (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[widgetId].wdgtType))
    {
        result = CapSense_dsRam.snsStatus[widgetId];
        result >>= CapSense_PROX_STS_OFFSET(proxId);
        result &=  CapSense_PROX_STS_MASK;
    }
    
    return result;
}
#endif /* #if (0u != CapSense_PROXIMITY_WIDGET_EN) */


#if (0u != CapSense_SLIDER_WIDGET_EN)
/*******************************************************************************
* Function Name: CapSense_GetCentroidPos
****************************************************************************//**
*
* \brief
*  Reports the centroid position for the specified slider widget.
*
* \details 
*  This function reports the centrode value of a specified radial or linear 
*  slider widget by extracting the information from the wdgtStatus registers 
*  (CapSense_<WidgetName>_POSITION<X>_VALUE). This function should be 
*  used only with the radial or linear slider widgets. This function does not 
*  process the widget but extracts the processed results from the \ref group_structures.
*
* \param widgetId 
*  Specify the ID number of the slider widget to get the centroid of the 
*  detected touch.
*  A macro for the widget ID can be found in the 
*  CapSense Configuration header file defined as 
*  CapSense_<WidgetName>_WDGT_ID
*
* \return 
*  Returns the centroid position of a specified slider widget:
*    - The centroid position if a touch is detected.
*    - CapSense_SLIDER_NO_TOUCH if no touch is detected or a wrong 
*      widgetId is specified.
*
*******************************************************************************/
uint32 CapSense_GetCentroidPos(uint32 widgetId)
{
    uint32 result = CapSense_SLIDER_NO_TOUCH;
    CapSense_RAM_WD_SLIDER_STRUCT *wdSlider;
    
    if ((widgetId < CapSense_TOTAL_WIDGETS) && 
        ((CapSense_WD_LINEAR_SLIDER_E == 
            (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[widgetId].wdgtType) ||
         (CapSense_WD_RADIAL_SLIDER_E == 
            (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[widgetId].wdgtType)))
    {
        wdSlider = CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;
        result = (uint32)wdSlider->position[0u];
    }
    
    return result;
}
#endif /* #if (0u != CapSense_SLIDER_WIDGET_EN) */


#if (0u != CapSense_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CapSense_GetXYCoordinates
****************************************************************************//**
*
* \brief
*  Reports the X/Y position detected for the specified touchpad widget.
*
* \details 
*  This function reports a touch position (X and Y coordinates) value of a 
*  specified touchpad widget by extracting the information from the wdgtStatus 
*  registers (CapSense_<WidgetName>_POS_Y_VALUE). This function should 
*  be used only with the touchpad widgets. This function does not process the 
*  widget, but extracts the processed results from the \ref group_structures.
*
* \param widgetId 
*  Specify the ID number of the touchpad widget to get the X/Y position of a 
*  detected touch.
*  A macro for the widget ID can be found in the 
*  CapSense Configuration header file defined as 
*  CapSense_<WidgetName>_WDGT_ID.
*
* \return 
*  Returns the touch position of a specified touchpad widget:
*    1. If a touch is detected:
*      - Bits [31..16] indicates the Y coordinate.
*      - Bits [15..0] indicates the X coordinate.
*    2. If no touch is detected or a wrong widgetId is specified:
*      - CapSense_TOUCHPAD_NO_TOUCH.
*
*******************************************************************************/
uint32 CapSense_GetXYCoordinates(uint32 widgetId)
{
    CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt = NULL;
    
    #if (0u != CapSense_CSD_TOUCHPAD_WIDGET_EN)
        CapSense_RAM_WD_CSD_TOUCHPAD_STRUCT *wdCsdTouchpad;
    #endif /* #if (0u != CapSense_CSD_TOUCHPAD_WIDGET_EN) */
    
    #if (0u != CapSense_CSX_TOUCHPAD_WIDGET_EN)
        CapSense_RAM_WD_CSX_TOUCHPAD_STRUCT *wdCsxTouchpad;
    #endif /* #if (0u != CapSense_CSX_TOUCHPAD_WIDGET_EN) */
    
    cystatus state = CYRET_SUCCESS;
    uint32 result = CapSense_TOUCHPAD_NO_TOUCH;

    if (widgetId >= CapSense_TOTAL_WIDGETS)
    {
        state = CYRET_BAD_PARAM;
    }
    else
    {
        ptrFlashWdgt = &CapSense_dsFlash.wdgtArray[widgetId];
         
        if ((CapSense_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType != CapSense_WD_TOUCHPAD_E)
        {
            state = CYRET_BAD_PARAM;
        }
    }
      
    if (CYRET_SUCCESS == state)
    {
        switch(CapSense_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapSense_CSD_TOUCHPAD_WIDGET_EN)
            case CapSense_SENSE_METHOD_CSD_E:
                wdCsdTouchpad = ptrFlashWdgt->ptr2WdgtRam;
                
                result = ((uint32)wdCsdTouchpad->posX) |
                         ((uint32)wdCsdTouchpad->posY << 16u);
                break;
        #endif /* #if (0u != CapSense_CSD_TOUCHPAD_WIDGET_EN) */
        
        #if (0u != CapSense_CSX_TOUCHPAD_WIDGET_EN)
            case CapSense_SENSE_METHOD_CSX_E:
                wdCsxTouchpad = ptrFlashWdgt->ptr2WdgtRam;
                
                result = ((uint32)wdCsxTouchpad->touch[0u].x) |
                         ((uint32)wdCsxTouchpad->touch[0u].y << 16u);
                break;
        #endif /* #if (0u != CapSense_CSX_TOUCHPAD_WIDGET_EN) */
        
        default:
            CYASSERT(0u);
            break;
        }
    }
    
    return result;
}
#endif /* #if (0u != CapSense_TOUCHPAD_WIDGET_EN) */


/* [] END OF FILE */
