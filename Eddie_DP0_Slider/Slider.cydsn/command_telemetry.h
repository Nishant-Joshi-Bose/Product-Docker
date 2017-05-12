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
#ifndef COMMAND_TELEMETRY_H
#define COMMAND_TELEMETRY_H

#include <project.h>
#include "util.h"
#include "animation.h"

//=============================================================================
//======================================================== types and structures
//=============================================================================
typedef enum enum_alarm_type
{
    ALARM_LOG = 0,
    ALARM_INFO   ,
    ALARM_WARNING,
    ALARM_ERROR  ,
    ALARM_FATAL  ,
    ALARM_UNKNOWN// keep it last, not an alarm type
}t_enum_alarm_type;

typedef enum enum_command
{
    CMD_ANIMATION ,
    CMD_BOOTLOADER,
    CMD_CAPSENSE  ,
    CMD_LED       ,
    CMD_HELP      ,
    CMD_SENSOR    ,
    CMD_TELEMETRY ,
    CMD_TRACKER   ,
    CMD_VERSION   ,
    CMD_UNKNOWN   // keep it last, not a command
}t_enum_command;

typedef enum enum_anim_command
{
    CMD_ANIM_AUTO = 0,
    CMD_ANIM_MANUAL  ,
    CMD_ANIM_STOP    ,
    CMD_ANIM_RESUME  , 
    CMD_ANIM_UNKNOWN  // keep it last
}t_enum_anim_command;

typedef enum enum_sensor_command
{
    CMD_SENSOR_ENABLE = 0,
    CMD_SENSOR_DISABLE   ,
    CMD_SENSOR_UNKNOWN   // keep it last
}t_enum_sensor_command;

typedef enum enum_telemetry_command
{
    CMD_TELEMETRY_ENABLE = 0,
    CMD_TELEMETRY_DISABLE   ,
    CMD_TELEMETRY_UNKNOWN   // keep it last
}t_enum_telemetry_command;

typedef enum enum_tracker_command
{
    CMD_TRACKER_ENABLE = 0,
    CMD_TRACKER_DISABLE   ,
    CMD_TRACKER_SET_BUTTON,
    CMD_TRACKER_UNKNOWN   // keep it last
}t_enum_tracker_command;

typedef enum enum_led_command
{
    CMD_LED_CLEAR = 0      ,   
    CMD_LED_SET_COLOR_LEFT ,
    CMD_LED_SET_COLOR_RIGHT,
    CMD_LED_SET_WHITE      ,
    CMD_LED_SET_STRIP      ,
    CMD_LED_UNKNOWN
}t_enum_led_command;

typedef enum enum_bootloader_command
{
    CMD_BOOT_START = 0    ,
    CMD_BOOT_EXIT         ,
    CMD_BOOT_GET_FLASH_GEO, 
    CMD_BOOT_UNKNOWN
}t_enum_bootloader_command;

typedef struct struct_command
{
    uint16      id  ; 
    const char* name;
    const char* description;
}t_struct_command;
    
//=============================================================================
//========================================================= function prototypes
//============================================================================= 
void send_telemetry_sensors           (void);
void send_telemetry                   (const char* telemetry);
void send_animation_telemetry         (t_enum_animation animation_index, const char* action);
void send_button_telemetry            (int button, const char* action, t_enum_direction finger_direct, uint16 actual_raw_pos, uint16 previous_raw_pos);
void send_bootloader_config_telemetry (void);
void send_alarm_telemetry             (t_enum_alarm_type alarm_type, const char* source, const char* description); 
void send_flash_geometry_telemetry    (uint32 flash_checksum, uint32 flash_first_row, uint32 flash_last_row, uint32 flash_size);
BOOL parse_and_execute_command        (const char* command_str);
BOOL get_is_telemetry_enable          ();
void set_is_telemetry_enable          (BOOL is_telemetry_enable);
BOOL get_is_sensor_telemetry_enable   ();
void set_is_sensor_telemetry_enable   (BOOL is_sensor_telemetry_enable);
void send_push_button_telemetry       (uint8 capsense_id, char* button_name, BOOL is_button_down);

#endif // COMMAND_TELEMETRY_H    

/* [] END OF FILE */
