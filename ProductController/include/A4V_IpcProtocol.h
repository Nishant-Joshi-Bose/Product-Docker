/*
  File: A4V_IpcProtocol.h
  Author: Dillon Johnson
  Created: 12/18/2015
  Copyright: (C) 2015 Bose Corporation, Framingham, MA

  Description:
    Data structures and enumerations for A4V IPC.
*****************************************************************************
*/

#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#include <limits.h>

#include "A4V_IpcPlatformConfig.h"

#ifndef ipc_uint8_t
#error ipc_uint8_t must be defined.
#endif
#ifndef ipc_int8_t
#error ipc_int8_t must be defined.
#endif
#ifndef ipc_uint16_t
#error ipc_uint16_t must be defined.
#endif
#ifndef ipc_int16_t
#error ipc_int16_t must be defined.
#endif
#ifndef ipc_uint32_t
#error ipc_uint32_t must be defined.
#endif
#ifndef ipc_int32_t
#error ipc_int32_t must be defined.
#endif
#ifndef IPC_ALIGN_PACKET
#error IPC_ALIGN_PACKET must be defined.
#endif

#define IPC_NUM_DATA_BYTES 120U
#define IPC_NUM_EXTRA_BYTES 2
#define IPC_NO_ARG 0

/*****************************************************************************
 *                               Message Format
 */
typedef union
{
    struct _IpcPacket_t
    {
#ifdef A4V_IPC_BIG_ENDIAN
        ipc_uint8_t sender: 4;
        ipc_uint8_t destination: 4;
#else
        ipc_uint8_t destination: 4;
        ipc_uint8_t sender: 4;
#endif
        ipc_uint8_t opcode;
        ipc_uint8_t params[IPC_NUM_EXTRA_BYTES];

        union
        {
            ipc_uint8_t b[IPC_NUM_DATA_BYTES];
            ipc_uint32_t w[IPC_NUM_DATA_BYTES / sizeof( ipc_uint32_t )];
        } data;
        ipc_uint32_t checkXOR;
    } s;
    ipc_uint32_t  w[sizeof( struct _IpcPacket_t ) / sizeof( ipc_uint32_t )];
    ipc_uint8_t   b[sizeof( struct _IpcPacket_t ) / sizeof( ipc_uint8_t )];
} IPC_ALIGN_PACKET IpcPacket_t;

/*****************************************************************************
 *                                 Device Id
 */
typedef enum
{
    IPC_DEVICE_INVALID = 0,
    IPC_DEVICE_DSP = 1,
    IPC_DEVICE_LPM = 2,
    IPC_DEVICE_AP = 3,
    IPC_DEVICE_REMOTE = 4,
    IPC_DEVICE_F0 = 5,
    IPC_DEVICE_SOUNDTOUCH = 6,
    IPC_NUM_DEVICES
} Ipc_Device_t;

/*****************************************************************************
 *                                  Opcodes
 */
enum
{
    /* General */
    IPC_ECHO                    = 0x01,
    IPC_ECHO_RESPONSE           = 0x02,
    IPC_ERROR                   = 0x03,
    IPC_LOG_MESSAGE             = 0x04,

    /* Key */
    IPC_KEY                     = 0x05,
    IPC_KEY_HOOK                = 0x06,

    /* Log */
    IPC_LOG_EVT                 = 0x07,

    /* Polite Reboot */
    IPC_REBOOT_PLEASE           = 0x08,

    /* TAP Passthrough */
    IPC_PASSTHROUGH_SEND        = 0x09,
    IPC_PASSTHROUGH_RESPONSE    = 0x0A,

    /* Hamnmer Test */
    IPC_HAMMER                  = 0x0B,

    /* Factory Default */
    IPC_FACTORY_DEFAULT         = 0x0C,

    /* Shutdown Request */
    IPC_SHUTDOWN                = 0x0D,

    /* Status response msg */
    IPC_OPERATION_COMPLETE      = 0x0F,

    /* Health and Status */
    IPC_HOLD_OFF                = 0x10,
    IPC_STAT_REQ                = 0x11,
    IPC_DSP_H_S                 = 0x12,
    IPC_ST_H_S                  = 0x13,
    IPC_LPM_H_S                 = 0x14,
    IPC_RMT_H_S                 = 0x15,
    IPC_ASOC_H_S                = 0x16,
    IPC_LLW_H_S                 = 0x17,
    IPC_F0_H_S                  = 0x18,
    IPC_SYS_TEMP                = 0x19,
    IPC_OUT_OF_BOX_DONE         = 0x1A,
    IPC_SET_NEW_LANGUAGE        = 0x1B,

    /* Bulk */
    IPC_BULK_NOTIFY             = 0x20,
    IPC_BULK_RESPONSE           = 0x21,
    IPC_BULK_DATA               = 0x22,
    IPC_BULK_STATUS             = 0x23,
    IPC_BULK_QUERY              = 0x24,
    IPC_BULK_CLOSE              = 0x25,
    IPC_BULK_CLOSED             = 0x26,

    /* Update */
    IPC_UPDATE_AVAILABLE        = 0x30,
    IPC_UPDATE_ENTER            = 0x31,
    IPC_UPDATE_COMPLETE         = 0x32,
    IPC_UPDATE_DOWNLOAD         = 0x33,
    IPC_UPDATE_START_FLASH      = 0x34,
    IPC_UPDATE_AUTHENTICATE     = 0x35,
    IPC_UPDATE_INC_PROG         = 0x36,
    IPC_UPDATE_UNIFIED_PROG     = 0x37,
    IPC_UPDATE_STATUS           = 0x38,
    IPC_UPDATE_GET_STATUS       = 0x39,
    IPC_UPDATE_ACCESSORY_STARTING = 0x3A,

    /* RF Remote Control */
    IPC_RMT_LL_CTRL             = 0x40,
    IPC_RMT_DEL_BLASTER_RECORD  = 0x41,

    /* Source Control */
    IPC_SOURCE_ACTIVATE          = 0x50,
    IPC_SOURCE_ACTIVATION_STATUS = 0x51,
    IPC_SOURCE_LIST              = 0x52,
    IPC_SOURCE_META              = 0x53,
    IPC_SOURCE_UPDATED           = 0x56,
    IPC_HOSP_GET                 = 0x57,
    IPC_HOSP_RESP                = 0x58,
    IPC_PWR_MACRO                = 0x59,

    /* DSP Control */
    IPC_DSP_REBOOT_TO_IMAGE     = 0x60,
    IPC_DSP_AIQ_CONTROL         = 0x61,
    IPC_AIQ_COEF                = 0x62,
    IPC_DSP_VOL_MUTE            = 0x63,
    IPC_DSP_PROFILE             = 0x64,
    IPC_DSP_PROFILE_RESP        = 0x65,
    IPC_DSP_AUDIO_INFO          = 0x66,
    IPC_DSP_PLAY_TONE           = 0x67,

    /* Remote View Control */

    /* Soundtouch Control */
    IPC_ST_CONTROL_SET_LOW_POWER                   = 0x80,
    IPC_ST_CONTROL_LED_CONTROL                     = 0x81,
    IPC_ST_CONTROL_BT                              = 0x82,
    IPC_ST_CONTROL_UPDATE                          = 0x83,
    IPC_ST_CONTROL_WIFI_RADIO                      = 0x84,
    IPC_ST_CONTROL_CONTENT_CONTROL                 = 0x85,
    IPC_ST_CONTROL_RADIO_STATUS                    = 0x86,
    IPC_ST_CONTROL_CONTENT_ERROR_STATUS            = 0x87,
    IPC_ST_CONTROL_SET_DSP_LIPSYNC_DELAY           = 0x88,
    IPC_ST_CONTROL_AUDIO_PATH_PRESENTATION_LATENCY = 0x89,
    IPC_ST_CONTROL_SEND_TONE_CONTROL               = 0x8A,
    IPC_ST_CONTROL_GET_TONE_CONTROL                = 0x8B,
    //IPC_UNUSED                                     = 0x8C,
    IPC_ST_CONTROL_SET_DSP_MODE                    = 0x8D,
    IPC_ST_CONTROL_SPEAKER_PAIRING_COMMAND         = 0x8E,
    IPC_ST_CONTROL_RECOVERY_UPDATE_STATUS          = 0x8F,

    /* F0 Control */
    IPC_CEC_MSG                 = 0xA0,
    IPC_CEC_LOGICAL_ADDRESS     = 0xA1,
    IPC_POWER_STATE_SET         = 0xA2,
    IPC_POWER_STATE_GET         = 0xA3,
    IPC_POWER_STATE_STATUS      = 0xA4,
    IPC_CEC_BULK_MSG            = 0xA5,
    IPC_ASOC_BOOT_SELECT        = 0xA6,
    IPC_HDMI_MON_STATE_SET      = 0xA7,
    IPC_HDMI_MON_STATE_GET      = 0xA8,
    IPC_HDMI_MON_STATE_STATUS   = 0xA9,

    /* Audio Control */
    IPC_AUDIO_STATUS                = 0xB0,
    IPC_AUDIO_MUTE                  = 0xB1,
    IPC_AUDIO_SET_VOL               = 0xB2,
    IPC_AUDIO_GET_SPEAKER_LIST      = 0xB3,
    IPC_AUDIO_RSP_SPEAKER_LIST      = 0xB4,
    IPC_AUDIO_SET_SPEAKER_LIST      = 0xB5,
    IPC_AUDIO_OPEN_SPEAKER_PAIRING  = 0xB6,
    // SPEAKER_CLOSED uses IPC_OPERATION_COMPLETE
    IPC_AUDIO_GET_FULL_SPEAKER_LIST = 0xB7,


    /* UEI Quickset */
    IPC_GET_CEC_DATA            = 0xD0,
    IPC_RSP_CEC_DATA            = 0xD1,
    IPC_GET_SPD_DATA            = 0xD2,
    IPC_RSP_SPD_DATA            = 0xD3,
    IPC_GET_EDID_DATA           = 0xD4,
    IPC_RSP_EDID_DATA           = 0xD5,
    IPC_GET_CEC_MODE            = 0xD6,
    IPC_RSP_CEC_MODE            = 0xD7,
    IPC_SET_CEC_MODE            = 0xD8,
    IPC_GET_HDMI_ASSIGN         = 0xD9,
    IPC_RSP_HDMI_ASSIGN         = 0xDA,
    IPC_SET_HDMI_ASSIGN         = 0xDB,

    /* ASOC */
    IPC_ASOC_EXEC_COMMAND           = 0xE0,
    IPC_ASOC_EXEC_RESPONSE          = 0xE1,
    IPC_ASOC_SM2_NEEDS_RECOVERY     = 0xE2,
    IPC_ASOC_PERFORM_PENDING_UPDATE = 0xE3,
    IPC_ASOC_ADAPTIQ_SETTINGS       = 0xE4,
    // Old depricated screensaver         ,
    IPC_ASOC_APP_VERSION            = 0xE6,
    IPC_ASOC_VERSION_REQ            = 0xE7,
    IPC_ASOC_VERSION_RESP           = 0xE8,
    IPC_TIMEOUT_CMD                 = 0xE9,
};

/*****************************************************************************
* Keys
*/
typedef struct // IPC_KEY
{
    ipc_uint32_t time;
    ipc_uint32_t producer;
    ipc_uint32_t room;
    ipc_uint32_t key;
    ipc_uint32_t state;
} IpcKey_t;

/*****************************************************************************
* Tap Passthrough
*/
enum
{
    IPC_TAP_PASSTHROUGH_NOT_FINAL = 0,
    IPC_TAP_PASSTHROUGH_FINAL     = 1
};

/*****************************************************************************
* Operation Complete
*/
typedef enum
{
    IPC_COMPLETE_SET_LANGUAGE,
    IPC_COMPLETE_UPDATE_SRC_LIST,
    IPC_COMPLETE_SET_CODESET,
    IPC_COMPLETE_DEL_CODESET,
    IPC_COMPLETE_FACTORY_DEFAULT,
    IPC_COMPLETE_SPEAKER_PAIRING,
}  IpcOpCompleteID_t;

typedef enum
{
    IPC_COMPLETE_STAT_OK,
    IPC_COMPLETE_STAT_ERROR,
    IPC_COMPLETE_OUT_OF_MEMORY,
    IPC_COMPLETE_STAT_NOT_EXISTS,
    IPC_COMPLETE_STAT_EXISTS,
    IPC_COMPLETE_STAT_WRITE_ERROR,
    IPC_COMPLETE_STAT_INV_ARGS,
    IPC_COMPLETE_STAT_TIMEOUT,
}  IpcCompleteStatus_t;

#define OP_COMPLETE_MSG_LENGTH  64

typedef struct // IPC_OPERATION_COMPLETE
{
    ipc_uint8_t opCompleteID;
    ipc_uint8_t opCompleteStatus;
    ipc_uint16_t reserved;
    char msg[OP_COMPLETE_MSG_LENGTH];
} IpcOperationCompletePayload_t;

/*****************************************************************************
 *                           Health and Status
 */
typedef enum
{
    COUNTRY_UNDEFINED,
    COUNTRY_US,
    COUNTRY_GB,
    COUNTRY_CN,
    COUNTRY_JP,
    COUNTRY_TW,
    COUNTRY_NUM,
} COUNTRY_VARIANT;

typedef enum
{
    REGION_UNDEFINED,
    REGION_US,
    REGION_GB,
    REGION_RESERVED_CN,
    REGION_JP,
    REGION_NUM,
} REGION_VARIANT;

typedef enum
{
    SYSTEM_COLOR_WHITE,
    SYSTEM_COLOR_BLACK,
    SYSTEM_COLOR_NUM,
} SYSTEM_COLOR;

#define IPC_DSP_SW_VERSION_LEN_BYTES 24 // in bytes
typedef struct // IPC_DSP_H_S
{
    /* Word 1-6 */
    /* DSP has 32-bit char's, so we have to size the string appropriately */
    char swVersion[IPC_DSP_SW_VERSION_LEN_BYTES * 8 / CHAR_BIT];

    /* Word 7 */
    ipc_uint32_t image         : 16;
    ipc_uint32_t activeSource  : 16;

    /* Word 8 */
    ipc_uint32_t powerState    : 8;
    ipc_uint32_t muteState     : 8;
    ipc_uint32_t volume        : 8;
    ipc_uint32_t energyPresent : 8;

    /* Word 9 */
    ipc_uint32_t uptimeMs      : 32;

    /* Word 10 */
    ipc_uint32_t ampFaultState : 8;
    ipc_uint32_t bassEnableRequest : 8;
    ipc_uint32_t aiqLastRun    : 8;
    ipc_uint32_t aiqInstalled  : 8;

    /* Word 11 */
    ipc_uint32_t minimumOutputLatencyMs : 16;
    ipc_uint32_t totalLatencyMs         : 16;

    /* Word 12 */
    ipc_uint32_t audioFormat            : 8;
    ipc_uint32_t sampleRate             : 8;
    ipc_uint32_t fullRangeChannels      : 8;
    ipc_uint32_t lfeChannels            : 8;

} IpcDspHealthStatusPayload_t;

typedef enum
{
    SAMPLE_RATE_UNSPECIFIED = 0x00,
    SAMPLE_RATE_8000_HZ,
    SAMPLE_RATE_11025_HZ,
    SAMPLE_RATE_12000_HZ,
    SAMPLE_RATE_16000_HZ,
    SAMPLE_RATE_22050_HZ,
    SAMPLE_RATE_24000_HZ,
    SAMPLE_RATE_32000_HZ,
    SAMPLE_RATE_44100_HZ,
    SAMPLE_RATE_48000_HZ,
    SAMPLE_RATE_64000_HZ,
    SAMPLE_RATE_88200_HZ,
    SAMPLE_RATE_96000_HZ,
    SAMPLE_RATE_128000_HZ,
    SAMPLE_RATE_176400_HZ,
    SAMPLE_RATE_192000_HZ
} NominalSampleRate_t;

typedef enum
{
    AUDIO_FORMAT_UNKNOWN = 0x00,
    AUDIO_FORMAT_PCM,
    AUDIO_FORMAT_AC3,
    AUDIO_FORMAT_DTS,
    AUDIO_FORMAT_AAC,
    AUDIO_FORMAT_MLP,
    AUDIO_FORMAT_EAC3
} InputAudioFormat_t;

#define IPC_DUAL_MONO_FORMAT 0xff

#define LATENCY_VALUE_UNKNOWN 0xFFFF

#define ST_H_S_VERSION_LENGTH 24
typedef struct // IPC_ST_H_S
{
    char swVersion[ST_H_S_VERSION_LENGTH];
} IpcStHealthStatusPayload_t;

// Bits in LPM status field
#define LPM_STATUS_CRITICALERROR 1

#define LPM_H_S_VERSION_LENGTH 32
#define LPM_H_S_SERIAL_LENGTH 32 // Just to be safe, even though we're using the 18 byte serial someone may decide they want the 31 byte one instead
typedef struct // IPC_LPM_H_S
{
    ipc_uint32_t image;
    char swVersion[LPM_H_S_VERSION_LENGTH];
    ipc_uint32_t power;
    ipc_uint32_t source;
    ipc_uint32_t volume;
    ipc_uint32_t mute;
    ipc_uint32_t uptime;

    ipc_uint8_t regionCode;
    ipc_uint8_t countryCode;
    ipc_uint8_t speakerPackage;
    ipc_uint8_t audioMode;

    ipc_int8_t bass;
    ipc_int8_t treble;
    ipc_int8_t center;
    ipc_int8_t surround;

    ipc_uint16_t minimumOutputLatencyMs;
    ipc_uint16_t totalLatencyMs;

    ipc_uint16_t lipSyncDelayMs;
    ipc_uint8_t systemColor;
    ipc_uint8_t language;

    char serial[LPM_H_S_SERIAL_LENGTH];

    ipc_uint8_t status;
    ipc_uint8_t outOfBoxStatus;
    ipc_uint8_t networkStatus; // brings us to 107 bytes

} IpcLpmHealthStatusPayload_t;

#define REMOTE_SW_VERSION_LENGTH 8

typedef enum
{
    REMOTE_IMAGE_A = 0,
    REMOTE_IMAGE_B
} RemoteImage_t;

typedef enum
{
    PAIRING_DISABLED = 0,
    PAIRING_ENABLED = 1,
} PairingMode_t;

typedef enum
{
    PAIRING_NOT_PAIRED = 0,
    PAIRING_PAIRED = 1,
} PairingStatus_t;

typedef struct // IPC_RMT_H_S
{
    RemoteImage_t image;
    char rmtSwVersion[REMOTE_SW_VERSION_LENGTH];
    PairingMode_t pairimgMode;
    PairingStatus_t pairingStatus;
    ipc_uint32_t batteryStatus;
    ipc_uint32_t remoteAddress;
} RfRemoteHealthStatus_t;

typedef enum
{
    ASOC_NORMAL = 1,
    ASOC_RECOVERY,
    ASOC_FACTORY_TEST,
} ASOCImage_t;

typedef enum
{
    ASOC_MODE_PROD = 0,
    ASOC_MODE_DEV,
} ASOCMode_t;

#define ASOC_APP_VER_NAME_LEN           (20)
#define ASOC_APP_VERSION_LEN            (32)
#define ASOC_H_S_VERSION_LENGTH         (64)
#define ASOC_H_S_SYSTEM_VER_LENGTH      (20)
#define ASOC_H_S_LOGID_LENGTH           (20)
#define ASOC_H_S_STATUS_UPDATE_PENDING  (1 << 0)
#define ASOC_H_S_STATUS_APP_ALIVE       (1 << 1)

typedef struct // IPC_ASOC_H_S
{
    uint8_t unused0;                                //   0
    uint8_t statusFlags;                            //   1 uint8_t
    uint8_t mode;                                   //   2 ASOCMode_t
    uint8_t image;                                  //   3 ASOCImage_t
    char swVersion[ASOC_H_S_VERSION_LENGTH];        //   4
    char systemVersion[ASOC_H_S_SYSTEM_VER_LENGTH]; //  68
    char logID[ASOC_H_S_LOGID_LENGTH];              //  88
    // 108
} IpcAsocHealthStatusPayload_t;

typedef struct // IPC_F0_H_S
{
    char swVersion[24];
    char swBootloaderVersion[24];
    char swLongVersion[72];
} IpcF0HealthStatusPayload_t;

typedef enum
{
    GENERAL_STATUS = 0,
    RF_REMOTE_STATUS = 1,
    BLASTER_STATUS = 2,
    ASOC_STATUS = 3,
    LPM_STATUS = 4,
    DSP_STATUS = 5,
    LLW_STATUS = 6,
    ST_STATUS = 7,
    F0_STATUS = 8,
} StatusRequest_t;

typedef enum
{
    LPM_APP,
    LPM_RECOVERY,
} LPMImage_t;

typedef enum
{
    IPC_POWER_STATE_UNKNOWN       = 0x0,
    IPC_POWER_STATE_BOOTING       = 0x1,
    IPC_POWER_STATE_AUTO_WAKE     = 0x2,
    IPC_POWER_STATE_FULL_POWER    = 0x3,
    IPC_POWER_STATE_SHUTTING_DOWN = 0x4,
    IPC_POWER_STATE_OFF           = 0x5
} IpcPowerState_t;

typedef enum
{
    POWER_STATE_COLD_BOOTED = 0,
    POWER_STATE_LOW_POWER,
    POWER_STATE_NETWORK_STANDBY,
    POWER_STATE_AUTO_WAKE_STANDBY,
    POWER_STATE_FULL_POWER,
    POWER_STATE_NUM
} IpcLPMPowerState_t;


typedef enum
{
    IPC_NOT_MUTED = 0,
    IPC_MUTED     = 1
} IpcMuteState_t;

typedef enum
{
    IPC_ENERGY_NOT_PRESENT = 0,
    IPC_ENERGY_PRESENT     = 1
} IpcEnergyPresent_t;

typedef enum
{
    IPC_AIQ_COMPLETION_NOT_RUN      = 0x00,
    IPC_AIQ_COMPLETION_SUCCESSFUL   = 0x01,
    IPC_AIQ_COMPLETION_INCOMPLETE   = 0x02,
    IPC_AIQ_COMPLETION_UNSUCCESSFUL = 0x03
} IpcAiqCompletion_t;

typedef enum
{
    IPC_AIQ_NOT_INSTALLED   = 0,
    IPC_AIQ_INSTALLED       = ( 1 << 0 ),
    IPC_AIQ_ENABLED         = ( 1 << 1 )
} IpcAiqInstalled_t;

typedef enum
{
    END_USER_IMAGE,
    ERC_IMAGE,
    INSTALLER_IMAGE,
    DSP_NUM_IMAGES,
} DSPImage_t;

typedef enum
{
    IPC_THERM_CONSOLE_INTERNAL_AMP,
    IPC_THERM_CONSOLE_INTERNAL_PS,
    IPC_THERM_CENTER,           // Unused
    IPC_THERM_FRONT_SURROUND,   // Unused
    IPC_THERM_MID_SURROUND_L,   // Unused
    IPC_THERM_MID_SURROUND_R,   // Unused
    IPC_THERM_REAR_SURROUND_L,  // Current Maxwell left
    IPC_THERM_REAR_SURROUND_R,  // Current Maxwell right
    IPC_THERM_BASS,             // Current Skipper
    IPC_THERM_LOCATION_TYPES
} IpcThermistorLocation_t;

typedef struct
{
    ipc_uint16_t thermLocation; //ThermistorLocation
    ipc_int16_t tempDegC;
} IpcThermistorData_t;

typedef struct
{
    ipc_uint16_t reserved1;
    ipc_uint16_t numThermistorReadings;
    ipc_uint16_t reserved2;
    ipc_uint16_t reserved3;
    IpcThermistorData_t ThermistorData[IPC_THERM_LOCATION_TYPES];
} IpcSystemTemperatureData_t;

typedef struct  // SET_NEW_LANGUAGE
{
    ipc_uint32_t language      : 8;
    ipc_uint32_t reserved      : 24;
} IpcSetNewLangPayload_t;

/*****************************************************************************
 *                                Bulk
 */
#define BULK_FILE_NAME_MAX_SIZE 64 // in bytes
typedef struct // IPC_BULK_NOTIFY
{
    ipc_uint32_t length;
    ipc_uint32_t type;
    ipc_uint32_t fileId;
    /* DSP has 32-bit char's, so we have to size the string appropriately */
    char filename[BULK_FILE_NAME_MAX_SIZE * 8 / CHAR_BIT];
} BulkNotify_t;

typedef struct // IPC_BULK_RESPONSE
{
    ipc_uint32_t accept;
} BulkResponse_t;

typedef struct // IPC_BULK_STATUS
{
    ipc_uint32_t bufferAvailable;
    ipc_uint32_t status;
} BulkStatus_t;

typedef struct // IPC_BULK_CLOSE
{
    ipc_uint32_t status;
    ipc_uint32_t checksum;
} BulkClose_t;

typedef struct // IPC_BULK_CLOSED
{
    ipc_uint32_t status;
} BulkClosed_t;

enum // File types
{
    BULK_TYPE_INVALID              = 0,
    BULK_TYPE_SOFTWARE_UPDATE      = 1,
    BULK_TYPE_BLASTER_RECORD       = 2,
    BULK_TYPE_AIQ_COEFFICIENTS     = 3,
    BULK_TYPE_TEST                 = 4,
    BULK_TYPE_SOURCE_LIST          = 5,
    BULK_TYPE_EDID                 = 6,
    BULK_TYPE_ST_BT_DATA           = 7,
    BULK_TYPE_HOSPITALITY_FILE     = 8,
    BULK_TYPE_ACCESSORY_LIST       = 9,
    BULK_NUM_TYPES,
};

enum
{
    BULK_RESPONSE_ACCEPT  = 0x01,
    BULK_RESPONSE_DECLINE = 0x02
};

enum
{
    BULK_STATUS_CONTINUE = 1,
    BULK_STATUS_ERROR    = 2
};

enum
{
    BULK_CLOSE_OK    = 1,
    BULK_CLOSE_ERROR = 2
};

/*****************************************************************************
 *                               Update
 */
typedef struct // IPC_UPDATE_ENTER
{
    ipc_uint32_t mode;
} UpdateEnter_t;

typedef struct // IPC_UPDATE_INC_PROG
{
    ipc_uint32_t segment;
    ipc_uint32_t totalSegments;
    ipc_uint32_t percentComplete;
} UpdateIncProg_t;

typedef struct // IPC_UPDATE_UNIFIED_PROG
{
    ipc_uint32_t percentComplete;
} UpdateUnifiedProg_t;

typedef struct // IPC_UPDATE_STATUS
{
    ipc_uint32_t status;
} UpdateStatus_t;

enum // update statuses
{
    UPDATE_STATUS_NOT_IN_UPDATE,
    UPDATE_STATUS_READY_OK,
    UPDATE_STATUS_BUSY,
    UPDATE_STATUS_READY_FAIL,
    UPDATE_STATUS_AUTH_OK,
    UPDATE_STATUS_AUTH_FAIL,
    UPDATE_VERIFICATION_OK,
    UPDATE_VERIFICATION_FAIL,
    UPDATE_NUM_STATUSES,
};

/*****************************************************************************
 *                               Remote
 */
typedef enum
{
    RMT_CMD_INVALID = 0,
    SET_PAIRING_MODE = 1,
} RemoteCmd_t;

typedef enum
{
    PAIRING_DISABLE = 0,
    PAIRING_ENABLE = 1,
} PairingModeOpcode_t;

/*****************************************************************************
*                                Source Change
*/

//TODO - If adding or deleting a source from this list, make sure to verify that
//  the LPM enum SOURCE_ID is updated as needed.
typedef enum
{
    A4V_IPC_SOURCE_STANDBY = 0,
    A4V_IPC_SOURCE_HDMI_1,
    A4V_IPC_SOURCE_HDMI_2,
    A4V_IPC_SOURCE_HDMI_3,
    A4V_IPC_SOURCE_HDMI_4,
    A4V_IPC_SOURCE_HDMI_5,
    A4V_IPC_SOURCE_HDMI_6,
    A4V_IPC_SOURCE_SIDE_AUX,
    A4V_IPC_SOURCE_TV,
    A4V_IPC_SOURCE_SHELBY,
    A4V_IPC_SOURCE_BLUETOOTH,
    A4V_IPC_SOURCE_ADPAPTIQ,
    A4V_IPC_SOURCE_RESERVED1,   // SOURCE_DEMO for LPM
    A4V_IPC_SOURCE_PTS,
    A4V_IPC_SOURCE_UPDATE,
    A4V_IPC_SOURCE_ANALOG_FRONT,
    A4V_IPC_SOURCE_ANALOG1,
    A4V_IPC_SOURCE_ANALOG2,
    A4V_IPC_SOURCE_COAX1,
    A4V_IPC_SOURCE_COAX2,
    A4V_IPC_SOURCE_OPTICAL1,
    A4V_IPC_SOURCE_OPTICAL2,
    A4V_IPC_SOURCE_UNIFY,
    A4V_IPC_SOURCE_ASOC_INTERNAL,
    A4V_IPC_SOURCE_RESERVED2,    // SOURCE_DARR for LPM
    A4V_IPC_SOURCE_RESERVED3,    // SOURCE_LFE for LPM
    A4V_IPC_SOURCE_RESERVED4,    // SOURCE_WIRED for LPM
    A4V_IPC_SOURCE_RESERVED5,    // SOURCE_COLDBOOT for LPM
    A4V_IPC_SOURCE_FACTORY_DEFAULT,
    A4V_IPC_NUM_SOURCES,
    A4V_IPC_INVALID_SOURCE = A4V_IPC_NUM_SOURCES
}  A4V_IPC_SOURCE_ID;

typedef struct // IPC_SOURCE_ACTIVATE
{
    A4V_IPC_SOURCE_ID source;
    ipc_uint32_t open_field;
    ipc_uint32_t status;
} IPCSource_t;

// status for SOURCE_ACTIVATION_STATUS
typedef enum
{
    IPC_SOURCE_ACTIVATION_STATUS_NACK   = 0,
    IPC_SOURCE_ACTIVATION_STATUS_ACK    = 1,
    IPC_SOURCE_ACTIVATION_STATUS_FAIL   = 2,
    IPC_SOURCE_ACTIVATION_STATUS_DONE   = 3
} IPCSourceAckStatus_t;

typedef enum
{
    A4V_IPC_SRCTYPE_CABLE_BOX = 0,
    A4V_IPC_SRCTYPE_SAT_BOX,
    A4V_IPC_SRCTYPE_HDTV_TUNER,
    A4V_IPC_SRCTYPE_DIG_TV_TUNER,
    A4V_IPC_SRCTYPE_VCR,
    A4V_IPC_SRCTYPE_DVR,
    A4V_IPC_SRCTYPE_TIVO,
    A4V_IPC_SRCTYPE_SET_TOP_BOX,
    A4V_IPC_SRCTYPE_APPLE_TV,
    A4V_IPC_SRCTYPE_VUDO,
    A4V_IPC_SRCTYPE_DVD_PLAYER,
    A4V_IPC_SRCTYPE_BLURAY_PLAYER,
    A4V_IPC_SRCTYPE_DVD_RECORDER,
    A4V_IPC_SRCTYPE_BLURAY_RECORDER,
    A4V_IPC_SRCTYPE_DVD_VCR,
    A4V_IPC_SRCTYPE_CD_PLAYER,
    A4V_IPC_SRCTYPE_DIG_MEDIA_PLAYER,
    A4V_IPC_SRCTYPE_PLAYSTATION,
    A4V_IPC_SRCTYPE_XBOX,
    A4V_IPC_SRCTYPE_WII,
    A4V_IPC_SRCTYPE_GAME_CONSOLE,
    A4V_IPC_SRCTYPE_ROKU,
    A4V_IPC_SRCTYPE_COMPUTER,
    A4V_IPC_SRCTYPE_IPOD,
    A4V_IPC_SRCTYPE_RADIO_TUNER,
    A4V_IPC_SRCTYPE_XBOX_ONE,
    A4V_IPC_SRCTYPE_XBOX_360,
    A4V_IPC_SRCTYPE_AMAZON_FIRE_TV,
    A4V_IPC_SRCTYPE_CHROMECAST,
    A4V_IPC_SRCTYPE_BOSE_INTERNAL,
    A4V_IPC_SRCTYPE_UNDEFINED,
} A4V_IPCSrcType_t;

typedef enum
{
    A4V_IPC_CONTENT_UNSPECIFIED = 0,
    A4V_IPC_CONTENT_VIDEO,
    A4V_IPC_CONTENT_AUDIO,
} A4V_IPCContent_t;

// Note: the AudioInputSelection enum represents the
// number of bit shifts for the DSP defined values
// for audio input (see DSPSource_t definitions below)
// KEEP THESE IN SYNC WITH EACH OTHER
typedef enum
{
    A4V_IPC_AUDIO_INPUT_HDMI = 0,
    A4V_IPC_AUDIO_INPUT_ANALOG_AUD_1,
    A4V_IPC_AUDIO_INPUT_ANALOG_AUD_2,
    A4V_IPC_AUDIO_INPUT_ANALOG_FRONT,
    A4V_IPC_AUDIO_INPUT_MIC,
    A4V_IPC_AUDIO_INPUT_SPDIF_OPTICAL_1,
    A4V_IPC_AUDIO_INPUT_SPDIF_OPTICAL_2,
    A4V_IPC_AUDIO_INPUT_SPDIF_COAX_1,
    A4V_IPC_AUDIO_INPUT_SPDIF_COAX_2,
    A4V_IPC_AUDIO_INPUT_SPDIF_ARC,
    A4V_IPC_AUDIO_INPUT_SPDIF_NETWORK,
    A4V_IPC_AUDIO_INPUT_SPDIF_AP,
    A4V_IPC_AUDIO_INPUT_MAX_NUM,
    A4V_IPC_AUDIO_INPUT_DEFAULT = 0xFF,
} IPCAudioInputSelection_t;

#define IPC_SRC_RESERVED_LENGTH 18
typedef struct ipc_source
{
    A4V_IPC_SOURCE_ID         sourceID;
    char                      ueiKeyRecord[8];
    ipc_uint8_t               sourceType;
    ipc_uint8_t               sourceTypeSeqNum;
    ipc_uint8_t               contentType;
    ipc_uint8_t               AVSyncDelay;
    ipc_uint8_t               bassLevel;
    ipc_uint8_t               trebleLevel;
    ipc_uint8_t               audioMode;
    bool                      displayInList;
    ipc_uint8_t               audioInput;
    ipc_uint8_t               audioTrack;
    char                      reserved[IPC_SRC_RESERVED_LENGTH];
} IPCSourceStruct;

typedef struct
{
    uint32_t        numSources;
    IPCSourceStruct sourceListItems[0];
} IPCSourceListMessage;

typedef enum
{
    IPC_PWR_MACRO_GET = 0,
    IPC_PWR_MACRO_SET = 1,
    IPC_PWR_MACRO_RSP = 2,
} IPC_PowerMacro_Cmd_t;

typedef struct
{
    IPC_PowerMacro_Cmd_t cmd : 8;
    ipc_uint8_t enabled : 8;
    A4V_IPC_SOURCE_ID src : 8;
} IPC_PowerMacro_t;

/*****************************************************************************
 * DSP Profile
 */
typedef enum
{
    IPC_AIQ_CURVE_A      = 0x0,
    IPC_AIQ_CURVE_B      = 0x1,
    IPC_AIQ_CURVE_RETAIL = 0x2,
    IPC_AIQ_CURVE_RETAIL_FLOOR = IPC_AIQ_CURVE_RETAIL,
    IPC_AIQ_CURVE_SHELF        = 0x03
} IpcAiqCurveSelect_t;

typedef enum
{
    IPC_AMP_NO_FAULT    = 0,
    IPC_AMP_MINOR_FAULT = 1,
    IPC_AMP_MAJOR_FAULT = 2
} IpcAmpFaultState_t;

typedef enum
{
    IPC_DUAL_MONO_BOTH  = 0x0,
    IPC_DUAL_MONO_LEFT  = 0x1,
    IPC_DUAL_MONO_RIGHT = 0x2
} IpcDualMonoMode_t;

typedef struct // IPC_DSP_VOL_MUTE
{
    ipc_uint16_t muteState;
    ipc_uint16_t muteProfile;
    ipc_uint16_t volume;
    ipc_uint16_t rampProfile;
} IpcDSPVolumePayload;

typedef enum
{
    IPC_FRONT_CONNECTOR_UNSPECIFIED,
    IPC_FRONT_CONNECTOR_INPUT,
    IPC_FRONT_CONNECTOR_HEADPHONE
} IpcFrontConnectorMode_t;


typedef struct // IPC_DSP_PROFILE
{
    ipc_uint32_t sourceSelect    : 16;//1
    ipc_uint32_t powerState      : 16;

    ipc_uint32_t networkLatencyMs : 16;//2
    ipc_uint32_t targetLatencyMs  : 16;

    ipc_uint32_t audioMode       : 8;
    ipc_uint32_t aiqEnable       : 8;
    ipc_uint32_t aiqCurveSelect  : 8;
    ipc_uint32_t dualMonoSelect  : 8;

    ipc_int32_t bassLevel       : 8;
    ipc_int32_t trebleLevel     : 8;
    ipc_int32_t centerLevel     : 8;
    ipc_int32_t surroundLevel   : 8;

    ipc_uint32_t languageSelect  : 8;
    ipc_uint32_t region          : 8;
    ipc_uint32_t speakerPackage  : 8;
    ipc_uint32_t accessoryFlags  : 8;

    ipc_uint32_t frontConnector : 8;
    ipc_uint32_t reserved       : 24;
} IpcDspProfilePayload_t;

typedef struct // IPC_DSP_AUDIO_INFO
{
    ipc_uint8_t aifData[10]; //!< Audio Info-Frame data. Refer to CEA861 specification for details.
    ipc_uint8_t csData[7]; //!< Channel Status data. Refer to IEC 60958-3 specification for details.
} IpcDSPAudioMetaDataPayload_t;

enum
{
    DSP_MUTEPROFILE_NONE,
    DSP_MUTEPROFILE_INTERNAL,
    DSP_MUTEPROFILE_SOURCE,
    DSP_MUTEPROFILE_TV,
    DSP_MUTEPROFILE_USER,
};

// Note: the DSPSource_t definitions below are represented by
// a single byte with the IPCAudioInputSelection_t enum defined
// above.
// KEEP THESE IN SYNC WITH EACH OTHER
#define DSP_SOURCE_NONE            0
#define DSP_SOURCE_HDMI            ( 1 <<  0 )
#define DSP_SOURCE_ANALOG_AUD_1    ( 1 <<  1 )
#define DSP_SOURCE_ANALOG_AUD_2    ( 1 <<  2 )
#define DSP_SOURCE_ANALOG_FRONT    ( 1 <<  3 )
#define DSP_SOURCE_ANALOG_MIC      ( 1 <<  4 )
#define DSP_SOURCE_SPDIF_OPTICAL_1 ( 1 <<  5 )
#define DSP_SOURCE_SPDIF_OPTICAL_2 ( 1 <<  6 )
#define DSP_SOURCE_SPDIF_COAX_1    ( 1 <<  7 )
#define DSP_SOURCE_SPDIF_COAX_2    ( 1 <<  8 )
#define DSP_SOURCE_SPDIF_ARC       ( 1 <<  9 )
#define DSP_SOURCE_SPDIF_NETWORK   ( 1 << 10 )
#define DSP_SOURCE_SPDIF_AP        ( 1 << 11 )
typedef ipc_uint16_t DSPSource_t;

typedef enum
{
    IPC_AUDIO_MODE_UNSPECIFIED = 0,
    IPC_AUDIO_MODE_DIRECT      = 1,
    IPC_AUDIO_MODE_NORMAL      = 2,
    IPC_AUDIO_MODE_DIALOG      = 3,
    IPC_AUDIO_MODE_NIGHT       = 4
} IpcAudioMode_t;

typedef enum
{
    // Bardeen/Ginger speaker package enumerations
    IPC_SPEAKER_PACKAGE_BARDEEN_OMNIVO = 0x50,
    IPC_SPEAKER_PACKAGE_BARDEEN_JEWELCUBES,
    IPC_SPEAKER_PACKAGE_GINGER,
    IPC_SPEAKER_PACKAGE_GINGERSKIPPER,
    IPC_SPEAKER_PACKAGE_GINGERMAXWELL,
    IPC_SPEAKER_PACKAGE_GINGERSKIPPERMAXWELL,
    IPC_SPEAKER_PACKAGE_GINGERLOVEY,
    IPC_SPEAKER_PACKAGE_GINGERLOVEYMAXWELL,
    //
    IPC_FIRST_VALID_SPEAKER_PACKAGE = IPC_SPEAKER_PACKAGE_BARDEEN_OMNIVO,
    IPC_LAST_VALID_SPEAKER_PACKAGE  = IPC_SPEAKER_PACKAGE_GINGERLOVEYMAXWELL
} IpcSpeakerPackage_t;

typedef enum
{
    IPC_NO_ACCESSORY            = 0,
    IPC_BASSBOX_WIRED           = ( 1 << 0 ),
    IPC_REAR_SPEAKERS_WIRED     = ( 1 << 1 )
} IpcAccessoryFlags_t;

typedef struct //IPC_DSP_PLAY_TONE
{
    /* Word 0 */
    unsigned int toneId       : 32;

    /* Word 1 */
    unsigned int locationBits : 16;
    unsigned int repeatCount  : 16;

    /* Word 2 */
    unsigned int minVolume    : 16;
    unsigned int maxVolume    : 16;
} IpcDspPlayTonePayload_t;

typedef enum
{
    IPC_DSP_TONE_FORCE_STOP           = 0x0,
    IPC_DSP_TONE_AUTO_WAKE_ENABLE     = 0x1,
    IPC_DSP_TONE_AUTO_WAKE_DISABLE    = 0x2,
    IPC_DSP_TONE_SPEAKER_CONNECT_LS   = 0x3,
    IPC_DSP_TONE_SPEAKER_CONNECT_RS   = 0x4,
    IPC_DSP_TONE_SPEAKER_CONNECT_LFE  = 0x5,
    IPC_DSP_TONE_BT_PAIRED            = 0x6,
    IPC_DSP_TONE_PAIRING_LIST_CLEARED = 0x7,
    IPC_DSP_TONE_PRESET_SET           = 0x8

} IpcDspToneId_t;

typedef enum
{
    IPC_TONE_LOCATION_L   = ( 1 << 0 ),
    IPC_TONE_LOCATION_R   = ( 1 << 1 ),
    IPC_TONE_LOCATION_LFE = ( 1 << 2 ),
    IPC_TONE_LOCATION_C   = ( 1 << 3 ),
    IPC_TONE_LOCATION_LS  = ( 1 << 4 ),
    IPC_TONE_LOCATION_RS  = ( 1 << 5 ),
    IPC_TONE_LOCATION_LB  = ( 1 << 6 ),
    IPC_TONE_LOCATION_RB  = ( 1 << 7 ),
    //
    IPC_TONE_LOCATION_ALL = 0xffff
} IpcToneLocationBits_t;  // Funky cold medina!

#define IPC_DSP_TONE_REPEAT_FOREVER 0xffff

typedef enum
{
    IPC_ST_NETWORKSTATUS_OFF = 0,
    IPC_ST_NETWORKSTATUS_WIFIAP,
    IPC_ST_NETWORKSTATUS_WIFI,
    IPC_ST_NETWORKSTATUS_WIFICONNECTING,
    IPC_ST_NETWORKSTATUS_ETHERNET,
} IpcSTNetworkStatus_t;

typedef enum
{
    IPC_ST_BTSTATUS_OFF = 0,
    IPC_ST_BTSTATUS_PAIRING,
    IPC_ST_BTSTATUS_CONNECTING,
    IPC_ST_BTSTATUS_CONNECTED,
    IPC_ST_BTSTATUS_PAIRING_LIST_CLEARED,
} IpcSTBluetoothStatus;



typedef struct
{
    ipc_uint32_t wlan0;
    ipc_uint32_t wlan1;
    ipc_uint8_t networkStatus;
    ipc_uint8_t btStatus;
    ipc_uint8_t loginStatus;
    ipc_uint8_t _open;
} IpcRadioStatus_t;

/*****************************************************************************
 *                         SoundTouch Control
 */

/* LED control
 */

typedef enum
{
    IPC_LED_CONTROL_COMMAND     = 0,
    IPC_LED_CONTROL_LED_NUMBER  = 1
} IpcLEDControlParams_t;

typedef enum
{
    IPC_LED_CONTROL_COMMAND_ON          = 0,
    IPC_LED_CONTROL_COMMAND_OFF         = 1,
    IPC_LED_CONTROL_COMMAND_BLINK       = 2,
    IPC_LED_CONTROL_COMMAND_PULSE       = 3,
    IPC_LED_CONTROL_COMMAND_RELINQUISH  = 4
} IpcLEDControlCommands_t;

typedef enum
{
    IPC_LED_CONTROL_LED_WIFI_WHITE      = 0,
    IPC_LED_CONTROL_LED_WIFI_AMBER      = 1,
    IPC_LED_CONTROL_LED_BT_WHITE        = 2,
    IPC_LED_CONTROL_LED_BT_BLUE         = 3,
    IPC_LED_CONTROL_LED_AUX_WHITE       = 4, // not used in Ginger
    IPC_LED_CONTROL_LED_SHELBY_WHITE    = 5,
    IPC_LED_CONTROL_LED_SHELBY_AMBER    = 6,
    IPC_LED_CONTROL_LED_TV_WHITE        = 7,
    IPC_LED_CONTROL_LED_TV_GREEN        = 8,
    IPC_LED_CONTROL_LED_BT_GREEN        = 9,
    IPC_LED_CONTROL_LED_HEADSET_WHITE   = 10,
    IPC_LED_CONTROL_LED_MAX_NUMBER      = 10
} IpcLEDControlLEDNumber_t;

typedef enum
{
    // The short version, with a single uint32_t argument
    IPC_LED_CONTROL_ARGUMENT_SHORT_INTENSITY = 0
} IpcLEDControlArgumentsShort_t;

typedef enum
{
    // The long version, with multiple uint16_t and uint8_t arguments; they are located in the uint8_t array "b"
    IPC_LED_CONTROL_ARGUMENT_LONG_ITERATIONS            = 0, // uint16_t
    IPC_LED_CONTROL_ARGUMENT_LONG_FINAL_MAX_INTENSITY   = 2, // uint8_t
    IPC_LED_CONTROL_ARGUMENT_LONG_DEPTH                 = 3, // uint8_t
    IPC_LED_CONTROL_ARGUMENT_LONG_DURATION              = 4, // uint16_t
    IPC_LED_CONTROL_ARGUMENT_LONG_INTENSITY             = 6  // uint16_t
} IpcLEDControlArgumentsLong_t;

enum
{
    IPC_LED_CONTROL_ARGUMENT_LONG_MAX_DEPTH         = 8  // taken from MAX_NUM_BLINK_PATTERNS in PRU.h
};

/* WIFI radio control
 */

typedef enum
{
    IPC_WIFI_RADIO_CONTROL_COMMAND_ON       = 0,
    IPC_WIFI_RADIO_CONTROL_COMMAND_OFF      = 1,
    IPC_WIFI_RADIO_CONTROL_COMMAND_TOGGLE   = 2,
} IpcWifiRadioContolCommand_t;

typedef enum
{
    IPC_WIFI_RADIO_CONTROL_TARGET_WIFI_RADIO   = 0,
    IPC_WIFI_RADIO_CONTROL_TARGET_AP_MODE      = 1,
} IpcWifiRadioContolTarget_t;

/* Content control
 */

typedef enum
{
    ST_CONTENT_CONTROL_COMMAND_PRESET_SELECT,
    ST_CONTENT_CONTROL_COMMAND_PRESET_SET,
    ST_CONTENT_CONTROL_COMMAND_PRESET_CLEAR,
    ST_CONTENT_CONTROL_COMMAND_CONTENT_CONTROL
}  IpcSTContentControlCommand_t;


typedef enum
{
    ST_CONTENT_CONTROL_ACTION_STOP,
    ST_CONTENT_CONTROL_ACTION_PLAY,
    ST_CONTENT_CONTROL_ACTION_PAUSE,
    ST_CONTENT_CONTROL_ACTION_PLAY_PAUSE,
    ST_CONTENT_CONTROL_ACTION_NEXT_TRACK,
    ST_CONTENT_CONTROL_ACTION_PREVIOUS_TRACK,
    ST_CONTENT_CONTROL_ACTION_RATE_UP,
    ST_CONTENT_CONTROL_ACTION_RATE_DOWN,
    ST_CONTENT_CONTROL_ACTION_RATE_NONE
} IpcSTContentControlAction_t;

typedef struct // IPC_ST_CONTROL_CONTENT_CONTROL
{
    IpcSTContentControlAction_t action;
} IpcContentControl_t;


/* Update control
 */

typedef enum
{
    IPC_CONTROL_DISPLAY_SOUNDTOUCH_UPDATE_STATE = 0,
    IPC_CONTROL_UPDATE_ATTEMPT_SYSTEM_UPDATE,
} IpcControlUpdateCommand_t;


typedef enum
{
    ST_STATUS_TYPE_CONTENT_STATUS,
    ST_STATUS_TYPE_ERROR_WARNING_STATUS,
} IpcStatusType_t;

typedef enum
{
    ST_CONTENT_STATUS_PLAYING,
    ST_CONTENT_STATUS_BUFFERING,
    ST_CONTENT_STATUS_PRESET_SET,
    ST_CONTENT_STATUS_AUDIO_SYNCING,
    ST_CONTENT_STATUS_OPERATION_ACK,
} IpcContentStatus_t;

typedef enum
{
    ST_ERROR_WARNING_NORMAL,
    ST_ERROR_WARNING_ERROR_EVENT,
    ST_ERROR_WARNING_ERROR_STATE,
    ST_ERROR_WARNING_ERROR_EVENT_FOLLOWED_BY_STATE,
    ST_ERROR_WARNING_CRITICAL_ERROR,
    ST_ERROR_WARNING_INFORMATIONAL,
} IpcErrorWarningStatus_t;

/* BT device data
 */


#define BT_MAC_LENGTH 6
typedef struct
{
    uint8_t macAddress[BT_MAC_LENGTH];
    char deviceName[114];
} IpcBTInfo_t;

/* Update display status
    These values are also used in SoftwareUpdateAPIMsgDefinitions::SoftwareUpdateStatus
 */

enum
{
    IPC_UPDATE_DISPLAY_STATUS_IDLE              = 1,
    IPC_UPDATE_DISPLAY_STATUS_DOWNLOADING       = 2,
    IPC_UPDATE_DISPLAY_STATUS_AUTHENTICATING    = 3,
    IPC_UPDATE_DISPLAY_STATUS_INSTALLING        = 4,
    IPC_UPDATE_DISPLAY_STATUS_COMPLETED         = 5
};

/* Lip Sync Setting

 */
typedef struct // IPC_ST_CONTROL_SET_DSP_LIPSYNC_DELAY
{
    ipc_uint32_t lipSyncDelay;
} IpcLipSyncDelay_t;

/* Audio Path Presentation Latency Setting

 */
typedef struct // IPC_ST_CONTROL_AUDIO_PATH_PRESENTATION_LATENCY
{
    ipc_uint32_t networkLatency;
} IpcAudioPathPresentationLatency_t;


/* used both for setting values
   and for sending the current values
   and the associated limits
 */
typedef struct
{
    ipc_int16_t bass;
    ipc_int16_t treble;
    ipc_int16_t centerSpeaker;
    ipc_int16_t surroundSpeaker;
    ipc_int16_t minBass;
    ipc_int16_t minTreble;
    ipc_int16_t minCenterSpeaker;
    ipc_int16_t minSurroundSpeaker;
    ipc_int16_t maxBass;
    ipc_int16_t maxTreble;
    ipc_int16_t maxCenterSpeaker;
    ipc_int16_t maxSurroundSpeaker;
    ipc_int16_t stepBass;
    ipc_int16_t stepTreble;
    ipc_int16_t stepCenterSpeaker;
    ipc_int16_t stepSurroundSpeaker;
} IpcToneControl_t;

/* Setting the DSP DRC Modes

 */
typedef struct // IPC_ST_CONTROL_SET_DSP_MODE
{
    ipc_int16_t audioMode; //uses IpcAudioMode_t
    ipc_uint8_t ExtraSpace[2];
} IpcSetDSPAudioMode_t;

/*****************************************************************************
*                            HDMI Control
*/
typedef struct
{
    ipc_uint32_t sourceID;
    ipc_uint32_t vendorID;
    ipc_uint32_t logicalAddress;
    ipc_uint8_t osdName[16];
} IpcResponseCECData_t;

typedef struct
{
    ipc_uint8_t vendorName[8];
    ipc_uint8_t productDescription[16];
    ipc_uint8_t sourceInformation;
} IpcResponseSPDInfoFrame_t;

typedef enum
{
    IPC_CEC_MODE_OFF = 0,
    IPC_CEC_MODE_ON,
    IPC_CEC_MODE_ALTENATIVE,
    IPC_CEC_MODE_FINE,
} IpcCECSelectModes_t;

typedef struct
{
    ipc_uint8_t cecMode; // IpcCECSelectModes_t
    ipc_uint8_t open[3];
    uint32_t cecModeFineSettings;
    uint32_t cecModeFineSupport;
} IpcResponseCECMode_t;

typedef enum
{
    IPC_HDMI_IN_NONE = 0,
    IPC_HDMI_IN_SOURCE_01,
    IPC_HDMI_IN_SOURCE_02,
    IPC_HDMI_IN_SOURCE_03,
    IPC_HDMI_IN_SOURCE_04,
} IpcHDMIInputSelection_t;

typedef struct
{
    ipc_uint8_t HDMIInputSelection; // IpcHDMIInputSelection_t
} IpcResponseHDMIInputSelection_t;






/*****************************************************************************
 *                            Audio Control
 */
typedef struct // IPC_AUDIO_STATUS
{
    ipc_uint32_t volume;
    ipc_uint32_t mute;
    ipc_uint32_t audioLatency;
} IpcAudioStatus_t;

typedef struct // IPC_AUDIO_MUTE
{
    ipc_uint32_t internalMute;
    ipc_uint32_t unifyMute;
} IpcAudioMute_t;

typedef enum
{
    IPC_AUDIO_MUTE_UNSPECIFIED,
    IPC_AUDIO_MUTE_ENABLE,
    IPC_AUDIO_MUTE_DISABLE,
} IpcAudioMuteState_t;

typedef struct // IPC_AUDIO_SET_VOL
{
    ipc_uint32_t volume;
} IpcAudioSetVolume_t;

typedef union
{
    struct speakerList
    {
        int frontRight   : 1;
        int frontLeft    : 1;
        int frontCenter  : 1;
        int rearRight    : 1;
        int rearLeft     : 1;
        int sub01        : 1;
        int sub02        : 1;
        int extraBits    : 25;
    } field;
    uint32_t data;
} IpcSpeakerList_t;

// Note - the above IpcSpeakerList_t bit-field definition conveys the
// intent of the bitfield for the speaker list, but according to K&R,
// may or may not compile/execute as intended based on compiler
// and/or platform implementation.  Therefore, it's probably best to
// use the following bit-masks to set and/or decode the speaker list.
#define IPC_SPEAKER_MASK_REAR 0x18000000
#define IPC_SPEAKER_MASK_SUB01 0x04000000
#define IPC_SPEAKER_MASK_SUB02 0x02000000

typedef enum
{
    ACCESSORY_SUB,
    ACCESSORY_SKIPPER = ACCESSORY_SUB,
    ACCESSORY_REAR_SURROUND,
    ACCESSORY_LOVEY,
    // ACC_ETC for future
    ACCESSORY_INVALID
} AccessoryType_t;

typedef enum
{
    ACCESSORY_POSITION_START,
    ACCESSORY_POSITION_SUB = ACCESSORY_POSITION_START,
    ACCESSORY_POSITION_LEFT_REAR,
    ACCESSORY_POSITION_RIGHT_REAR,
    ACCESSORY_POSITION_SUB_2,
    ACCESSORY_POSITION_NUM_OF_LEGACY = ACCESSORY_POSITION_SUB_2,
    ACCESSORY_POSITION_NUM_OF,
    ACCESORY_POSITION_INVALID = ACCESSORY_POSITION_NUM_OF
} AccessoryPosition_t;

typedef enum
{
    ACCESSORY_CONNECTION_NONE,
    ACCESSORY_CONNECTION_WIRED,
    ACCESSORY_CONNECTION_WIRELESS,
    ACCESSORY_CONNECTION_BOTH,
    ACCESSORY_CONNECTION_EXPECTED,
    ACCESSORY_CONNECTION_LEGACY_PAIRING,
} AccessoryConnectionStatus_t;

typedef enum
{
    ACCESSORY_ACTIVE_NOT_INITIALIZED = 0,
    ACCESSORY_DEACTIVATED = 1,
    ACCESSORY_ACTIVATED = 2,
} AccessoryActiveState_t;

typedef struct
{
    ipc_uint8_t type;
    ipc_uint8_t position;
    ipc_uint8_t status;
    ipc_uint8_t active; //whether it should play audio
    char sn[24]; //serial no
    char version[8]; //version 00.00.00
} AccessoryDescription_t;

typedef struct // IPC_CONNECTED_SPEAKER_PROFILE
{
    AccessoryDescription_t accessory[ACCESSORY_POSITION_NUM_OF_LEGACY];
} IpcAccessoryList_t;

typedef struct
{
    AccessoryDescription_t accessory[ACCESSORY_POSITION_NUM_OF];
} IpcAccessoryListFull_t;

/*****************************************************************************
 *                              F0 Power
 */
typedef enum
{
    POWER_DEVICE_INVALID,
    POWER_DEVICE_HDMI,
    POWER_DEVICE_SYSTEM,
    POWER_DEVICE_ASOC = IPC_DEVICE_AP,
    POWER_DEVICE_SM2 = IPC_DEVICE_SOUNDTOUCH,
    POWER_DEVICE_HDMI_FAIL,
    NUM_POWER_DEVICES,
} F0_PowerDevice_t;

typedef struct
{
    F0_PowerDevice_t device: 8;
    ipc_uint8_t enabled;
} F0_PowerStatus_t;

/*****************************************************************************
 *                              Soundtouch Bluetooth Sub-commands
 */
typedef enum
{
    IPC_ST_CONTROL_BT__SET_BT_PAIRING = 0x00,
    IPC_ST_CONTROL_BT__CLEAR_BT_PAIR_LIST = 0x01,
    IPC_ST_CONTROL_BT__BT_DATA = 0x02,
} ST_ControlBT_t;

typedef struct
{
    ipc_uint8_t cmd;
    ipc_uint8_t extra;
} IpcBT_Control_t;

typedef enum
{
    IPC_ST_CONTROL__DISPLAY_UPDATE_STATUS = 0x00,
    IPC_ST_CONTROL__ATTEMPT_SYSTEM_UPDATE = 0x01,
} ST_ControlUpdate_t;

/*****************************************************************************
 *                         Timeout cmd defs
 *                            IPC_TIMEOUT_CMD
 */
#define IPC_TIMEOUT_INACTIVITY       ( 1 << 0 )
#define IPC_TIMEOUT_AUDIOSILENCE     ( 1 << 1 )
#define IPC_TIMEOUT_USER_TIMEOUTS    (IPC_TIMEOUT_INACTIVITY|IPC_TIMEOUT_AUDIOSILENCE)
#define IPC_TIMEOUT_SCREENSAVER      ( 1 << 2 )
#define IPC_TIMEOUT_HOSP_BT_CLEAR    ( 1 << 3 )
#define IPC_TIMEOUT_ALL              ( IPC_TIMEOUT_INACTIVITY | IPC_TIMEOUT_AUDIOSILENCE | IPC_TIMEOUT_SCREENSAVER | IPC_TIMEOUT_HOSP_BT_CLEAR )

typedef enum
{
    TIMEOUT_CMD_INVALID     = 0, // Just ensure non null
    TIMEOUT_CMD_SET         = 1, // used for singular
    TIMEOUT_CMD_GET         = 2, // used to get singular
    TIMEOUT_CMD_RSP         = 3, // response to GET
    TIMEOUT_CMD_TRIGGERED   = 4, // Only from LPM (notifies device of timeout triggering)
    TIMEOUT_CMD_CLEARED     = 5, // Only from LPM (notifies device of timeout trigger being cleared)
    TIMEOUT_CMD_SET_ENABLED = 6, // Bulk set the bit fields for enabled
    TIMEOUT_CMD_GET_ENABLED = 7, // Bulk get the bit fields for enabled
    TIMEOUT_CMD_RSP_ENABLED = 8, // Bulk rsp with enable as bitfield of availible
} Ipc_TimeoutCommand_t;

typedef struct
{
    Ipc_TimeoutCommand_t cmd : 8;       // uint8
    ipc_uint8_t          enable;                 // 0 = disabled, 1 = enabled
    ipc_uint8_t          timeout_minutes; // number of minutes for time to be active
    A4V_IPC_SOURCE_ID    source : 8;      // uint8 active source (mainly for clear)
    ipc_uint8_t          timeout : 8;     // Timeout(s) to control
} Ipc_TimeoutControl_t;

/*****************************************************************************
 *                              Reboot Types
 */
typedef enum
{
    REBOOT_TYPE_SYSTEM = 0,  // this is the default and is used to request system reboot or indicate we are ready to be powered down(used for Factory default currently)
    REBOOT_TYPE_RECOVERY,    // device wants to go into recovery and may need to be power cycled
    REBOOT_TYPE_POWERDOWN,    // device is requesting the LPM to enter a lower power mode(is ready to be powered off), example ASOC determines remote has not paired within the required time limit
    REBOOT_TYPE_SHUTDOWN_ACK,  // LPM send a shutdown request, this is the ack from the device to indicate it is ok to power down
    REBOOT_TYPE_FACTORY_TEST,  // Device is requesting to go into factory test mode. LPM may need to power cycle device.
    REBOOT_TYPE_DEVICE_ONLY,   // Generic request to power cycle the device.
    REBOOT_TYPE_SM2_RECOVERY, // the ASOC will send this to reboot them to enter sm2 recovery, so this is our cue to start the recovery progress.
} IpcRebootType_t;


/*
 * This enum is used for IPC_ASOC_ADAPTIQ_SETTINGS
 * Each parameter can be set to no change, enabled, disabled
 */
typedef enum
{
    IPC_PARAM_NO_CHANGE,
    IPC_PARAM_ENABLE,
    IPC_PARAM_DISABLE,
} IpcTriStateParam;

/*****************************************************************************
 *                              Versions
 */

typedef enum
{
    VERSION_LPM,
    VERSION_BLOB,
    VERSION_DSP,
    VERSION_F0,
    VERSION_REMOTE_MAIN_FONT,
    VERSION_REMOTE_WIDGET_GRAPHIC,
    VERSION_ST,
    VERSION_SI9777,
    VERSION_MAXWELL_L,
    VERSION_MAXWELL_R,
    VERSION_BASS,
    VERSION_ASOC,
    VERSION_A4V_APP,
    VERSION_SYSTEM,
    VERSION_NUM_OF,
} IpcVersionType_t;

#define IPC_VERSION_VERS1_LEN   (60)
#define IPC_VERSION_VERS2_LEN   (36)
#define IPC_VERSION_VERS3_LEN   (20)

typedef struct
{
    uint8_t type;       //IpcVersionType_t
    uint8_t unused[3];
    char vers1[IPC_VERSION_VERS1_LEN];    // all 0s if none
    char vers2[IPC_VERSION_VERS2_LEN];
    char vers3[IPC_VERSION_VERS3_LEN];     // e.g. LLW - all 0s if none
} IpcVersionResponse_t;

#endif
