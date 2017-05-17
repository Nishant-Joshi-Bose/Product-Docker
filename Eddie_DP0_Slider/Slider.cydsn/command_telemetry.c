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
#include "command_telemetry.h"
#include "comms.h"

//=============================================================================
//============================================================ static variables
//=============================================================================
static t_struct_command vec_boot_commands[CMD_BOOT_UNKNOWN]           = {{CMD_BOOT_START         , "start"          , "enter in bootloader mode"          },
                                                                         {CMD_BOOT_EXIT          , "exit"           , "exit from bootloader mode"         },
                                                                         {CMD_BOOT_GET_FLASH_GEO , "get flash geo"  , "get flash geometry"                }};
static t_struct_command vec_anim_commands[CMD_ANIM_UNKNOWN]           = {{CMD_ANIM_AUTO          , "auto"           , "set animation in automatic mode"   },
                                                                         {CMD_ANIM_MANUAL        , "manual"         , "set animation in manual mode"      },
                                                                         {CMD_ANIM_STOP          , "stop"           , "stop animation"                    },
                                                                         {CMD_ANIM_RESUME        , "resume"         , "resume animation"                  }};
static t_struct_command vec_sensor_commands[CMD_SENSOR_UNKNOWN]       = {{CMD_SENSOR_DISABLE     , "disable"        , "disable sensors telemetries"       },
                                                                         {CMD_SENSOR_ENABLE      , "enable"         , "enable sensors telemetries"        }};  
static t_struct_command vec_telemetry_commands[CMD_TELEMETRY_UNKNOWN] = {{CMD_TELEMETRY_DISABLE  , "disable"        , "disable telemetries"               },
                                                                         {CMD_TELEMETRY_ENABLE   , "enable"         , "enable telemetries"                }};   
static t_struct_command vec_tracker_commands[CMD_TRACKER_UNKNOWN]     = {{CMD_TRACKER_DISABLE    , "disable"        , "disable finger tracker on LEDs"    },
                                                                         {CMD_TRACKER_ENABLE     , "enable"         , "enable finger tracker on LEDs"     },
                                                                         {CMD_TRACKER_SET_BUTTON , "set button"     , "set the number of logical buttons" }};                                                      
static t_struct_command vec_led_commands[CMD_LED_UNKNOWN]             = {{CMD_LED_CLEAR          , "clear"          , "clear all leds"                    },
                                                                         {CMD_LED_SET_COLOR_LEFT , "set color left" , "set the color of the left LED"     },
                                                                         {CMD_LED_SET_COLOR_RIGHT, "set color right", "set the color of the right LED"    },
                                                                         {CMD_LED_SET_WHITE      , "set white"      , "set the white LED intensity"       },
                                                                         {CMD_LED_SET_STRIP      , "set strip"      , "set the LED strip"                 }};
static t_struct_command vec_commands[CMD_UNKNOWN]                     = {{CMD_ANIMATION          , "animation"      , "set the animation parameters"      },
                                                                         {CMD_BOOTLOADER         , "bootloader"     , "jump in booloader mode"            },
                                                                         {CMD_CAPSENSE           , "capsense"       , "set/get the capsense configuration"},
                                                                         {CMD_HELP               , "help"           , ""                                  },
                                                                         {CMD_LED                , "led"            , "set led(s)"                        },
                                                                         {CMD_SENSOR             , "sensor"         , "enable/disable sensors telemetries"},
                                                                         {CMD_TELEMETRY          , "telemetry"      , "enable/disable telemetries"        },
                                                                         {CMD_TRACKER            , "tracker"        , "set/get the tracker configuration" },
                                                                         {CMD_VERSION            , "version"        , "get the actual version"            }};

static BOOL is_telemetry_enable        = FALSE; // enable/disable the telemetry
static BOOL is_sensor_telemetry_enable = FALSE; // when enable: send the sensors status on each capacitive touch pad activities.
static char telemetry[512];

//=============================================================================
//========================================================= function prototypes
//=============================================================================
t_enum_command            get_cmd_id_from_cmd_string          (const char* command_str);
t_enum_anim_command       get_anim_cmd_id_from_cmd_string     (const char* cmd_str);
t_enum_animation          get_anim_id_from_cmd_string         (const char* cmd_str);
t_enum_sensor_command     get_sensor_cmd_id_from_cmd_string   (const char* cmd_str);
t_enum_telemetry_command  get_telemetry_cmd_id_from_cmd_string(const char* cmd_str);
t_enum_tracker_command    get_tracker_cmd_id_from_cmd_string  (const char* cmd_str);
t_enum_led_command        get_led_cmd_id_from_cmd_string      (const char* cmd_str);
t_enum_bootloader_command get_boot_cmd_id_from_cmd_string     (const char* cmd_str);

//=============================================================================
//================================================== get_cmd_id_from_cmd_string
//=============================================================================
t_enum_command get_cmd_id_from_cmd_string(const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_commands[i].name))
        {
            return vec_commands[i].id;
        }
    }// for all the commands
    
    return CMD_UNKNOWN;
}// get_cmd_id_from_cmd_string

//=============================================================================
//=========================================== get_sensor_cmd_id_from_cmd_string
//=============================================================================
t_enum_sensor_command get_sensor_cmd_id_from_cmd_string (const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_SENSOR_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_sensor_commands[i].name))
        {
            return vec_sensor_commands[i].id;
        }
    }// for all the commands
    
    return CMD_SENSOR_UNKNOWN;
}// get_sensor_cmd_id_from_cmd_string

//=============================================================================
//======================================== get_telemetry_cmd_id_from_cmd_string
//=============================================================================
t_enum_telemetry_command get_telemetry_cmd_id_from_cmd_string (const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_TELEMETRY_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_telemetry_commands[i].name))
        {
            return vec_telemetry_commands[i].id;
        }
    }// for all the commands
    
    return CMD_TELEMETRY_UNKNOWN;
}// get_telemetry_cmd_id_from_cmd_string

//=============================================================================
//========================================== get_tracker_cmd_id_from_cmd_string
//=============================================================================
t_enum_tracker_command get_tracker_cmd_id_from_cmd_string(const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_TRACKER_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_tracker_commands[i].name))
        {
            return vec_tracker_commands[i].id;
        }
    }// for all the commands
    
    return CMD_TRACKER_UNKNOWN;
}// get_anim_cmd_id_from_cmd_string

//=============================================================================
//============================================= get_anim_cmd_id_from_cmd_string
//=============================================================================
t_enum_anim_command get_anim_cmd_id_from_cmd_string(const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_ANIM_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_anim_commands[i].name))
        {
            return vec_anim_commands[i].id;
        }
    }// for all the commands
    
    return CMD_ANIM_UNKNOWN;
}// get_anim_cmd_id_from_cmd_string

//=============================================================================
//================================================= get_anim_id_from_cmd_string
//=============================================================================
t_enum_animation get_anim_id_from_cmd_string(const char* cmd_str)
{
    int                i;
    t_struct_animation animation;
    
    for (i = 0; i < ANIM_UNKNOWN; i++)
    {
        get_animation ((t_enum_animation) i, &animation);
        if (strstr(cmd_str, animation.description))
        {
            return animation.id;
        }
    }// for all the commands
    
    return ANIM_UNKNOWN;
}// get_anim_cmd_id_from_cmd_string

//=============================================================================
//============================================= get_boot_cmd_id_from_cmd_string
//=============================================================================
t_enum_bootloader_command get_boot_cmd_id_from_cmd_string(const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_BOOT_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_boot_commands[i].name))
        {
            return vec_boot_commands[i].id;
        }
    }// for all the commands
    
    return CMD_BOOT_UNKNOWN;
}// get_boot_cmd_id_from_cmd_string

//=============================================================================
//============================================== get_led_cmd_id_from_cmd_string
//=============================================================================
t_enum_led_command get_led_cmd_id_from_cmd_string(const char* cmd_str)
{
    int i;
    
    for (i = 0; i < CMD_LED_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_led_commands[i].name))
        {
            return vec_led_commands[i].id;
        }
    }// for all the commands
    
    return CMD_LED_UNKNOWN;
}// get_led_cmd_id_from_cmd_string

//=============================================================================
//=================================================== parse_and_execute_command
//=============================================================================
BOOL parse_and_execute_command(const char* cmd_str_and_param)
{ 
    t_enum_command            cmd_id           = CMD_UNKNOWN;
    t_enum_anim_command       anim_cmd_id      = CMD_ANIM_UNKNOWN;
    t_enum_animation          anim_id          = ANIM_UNKNOWN;
    t_enum_led_command        led_cmd_id       = CMD_LED_UNKNOWN;
    t_enum_sensor_command     sensor_cmd_id    = CMD_SENSOR_UNKNOWN;
    t_enum_telemetry_command  telemetry_cmd_id = CMD_TELEMETRY_UNKNOWN;
    t_enum_tracker_command    tracker_cmd_id   = CMD_TRACKER_UNKNOWN;
    t_enum_bootloader_command boot_cmd_id      = CMD_BOOT_UNKNOWN;   
    char*                     param_str        = strchr(cmd_str_and_param, ',');
    char*                     next_param_str   = NULL; 
    int                       red              = 0;
    int                       green            = 0;
    int                       blue             = 0;
    int                       led_index        = 0;
    int                       led_intensity    = 0;
#if 0   
    uint32                    flash_checksum   = 0;
    uint32                    flash_first_row  = 0;
    uint32                    flash_last_row   = 0;
    uint32                    flash_size       = 0;
#endif // USE_BOOTLOADER    
    t_struct_animation        animation;
    uint16                    led_vec[MAX_LEDS]; 
    int                       i;
    
    if (param_str == NULL)
    {
        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't find comma in command");
        return FALSE;    
    }// If we can't a comma inside the command
   
    cmd_id = get_cmd_id_from_cmd_string(param_str++);
    
    switch (cmd_id)
    {
        case CMD_CAPSENSE:
        {
            send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "FIXME!");
            return FALSE;    
        }// CMD_CAPSENSE
        
        case CMD_HELP:
        {
            for (i = 0; i < CMD_UNKNOWN; i++)
            {
                send_alarm_telemetry(ALARM_LOG, vec_commands[i].name, vec_commands[i].description);
            }// for all the commands
            
            for (i = 0; i < ANIM_UNKNOWN; i++)
            {
                get_animation ((t_enum_animation) i, &animation);
                send_alarm_telemetry(ALARM_LOG, "animation", animation.description);
            }// for all the commands
            
            break;      
        }// CMD_HELP
        
        case CMD_ANIMATION:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action"); 
                return FALSE;
            }// If we can't find the next ',' character
            
            if ((param_str = strchr(++param_str, ':')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action"); 
                return FALSE;
            }// If we can't find the next ':' character
            param_str++;
            
            switch (anim_cmd_id = get_anim_cmd_id_from_cmd_string(param_str))
            {
                case CMD_ANIM_AUTO:
                {
                    set_is_auto_anim_rotation_enable(TRUE);
                    break;
                }// CMD_ANIM_AUTO
                
                case CMD_ANIM_MANUAL:
                {
                    set_is_auto_anim_rotation_enable(FALSE);
                    break;
                }// CMD_ANIM_MANUAL
                
                case CMD_ANIM_STOP:
                {
                    set_is_anim_on_button_up_enable(FALSE);
                    break;
                }// CMD_ANIM_STOP
                
                case CMD_ANIM_RESUME:
                {
                    set_is_anim_on_button_up_enable(TRUE);
                    break;
                }// CMD_ANIM_RESUME
                
                case CMD_ANIM_UNKNOWN:
                default              :
                {
                    if ((anim_id = get_anim_id_from_cmd_string(param_str)) != ANIM_UNKNOWN)
                    {
                        set_current_callback_anim(anim_id);
                    }// if we found a valid animation description in the command
                    else
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action"); 
                        return FALSE;
                    }// else, we couldn't find a valid animation 
                    break;
                }// CMD_ANIM_UNKOWN, default
            }// switch on the animation command
            break;  
        }// CMD_SET_ANIMATION
        
        case CMD_BOOTLOADER:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported sensor action"); 
                return FALSE;
            }// If we can't find the next ',' character
            
            if ((param_str = strchr(++param_str, ':')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported sensor action"); 
                return FALSE;
            }// If we can't find the next ':' character
            param_str++;
            
            switch (boot_cmd_id = get_boot_cmd_id_from_cmd_string(param_str))
            {
                case CMD_BOOT_START:
                {   

#if USE_BOOTLOADER                     
                    Timer_1_Stop();
                    Bootloadable_1_Load();
#else 
                    send_alarm_telemetry(ALARM_ERROR, cmd_str_and_param, "bootloader is disabled"); 
#endif // USE_BOOTLOADER  
                    break;
                }// CMD_BOOT_START      
                
                case CMD_BOOT_EXIT:
                {
#if 0                    
                    Bootloader_1_Exit(Bootloader_1_EXIT_TO_BTLDB);
#else 
                    send_alarm_telemetry(ALARM_ERROR, cmd_str_and_param, "bootloader is disabled"); 
#endif // USE_BOOTLOADER      
                    break;
                }// CMD_BOOT_EXIT
                
                case CMD_BOOT_GET_FLASH_GEO:
                {
#if 0                   
                    flash_checksum  = Bootloader_1_GetMetadata(Bootloader_1_GET_BTLDB_CHECKSUM, 0);
                    flash_first_row = Bootloader_1_GetMetadata(Bootloader_1_GET_BTLDB_ADDR    , 0);
                    flash_last_row  = Bootloader_1_GetMetadata(Bootloader_1_GET_BTLDR_LAST_ROW, 0);
                    flash_size      = Bootloader_1_GetMetadata(Bootloader_1_GET_BTLDB_LENGTH  , 0);
                    send_flash_geometry_telemetry(flash_checksum, flash_first_row, flash_last_row, flash_size);
#else 
                    send_alarm_telemetry(ALARM_ERROR, cmd_str_and_param, "bootloader is disabled"); 
#endif // USE_BOOTLOADER  
                    break;
                }// CMD_BOOT_EXIT
                
                case CMD_BOOT_UNKNOWN:
                default              :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported bootloader action"); 
                    return FALSE;
                }// CMD_TRACKER_UNKNOWN, default
            }// switch in the tracker action
            break;
            //send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "jumping in bootloader");             
           
            break;
        }// CMD_BOOTLOADER
              
        case CMD_LED:
        {
            switch (led_cmd_id = get_led_cmd_id_from_cmd_string(param_str))
            {
                case CMD_LED_CLEAR:
                {
                    set_all_leds_intensity(0x00);
                    break;
                }// CMD_LED_CLEAR
                
                case CMD_LED_SET_COLOR_LEFT:
                case CMD_LED_SET_COLOR_RIGHT:
                {
                    if ((param_str = strchr(param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action"); 
                        return FALSE;
                    }// If we can't find the next ',' character
                    
                    if ((param_str = strchr(++param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse red from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ':' character

                    // ========================================================
                    // parse red
                    // ========================================================
                    if ((param_str = strchr(++param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse red from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ':' character

                    if ((next_param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse red from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ',' character
                    *next_param_str = '\0';
                    next_param_str++;
                    
                    red = atoi(param_str);
                    
                    // ========================================================
                    // parse green
                    // ========================================================
                    if ((param_str = strchr(next_param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse green from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ':' character

                    if ((next_param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parsed green from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ',' character
                    *next_param_str = '\0';
                    next_param_str++;
                    
                    green = atoi(param_str);
                    
                    // ========================================================
                    // parse blue
                    // ========================================================
                    if ((param_str = strchr(next_param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse green from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ':' character
                    
                    blue = atoi(++param_str);                    
                    // ========================================================
                    // execute the command
                    // ========================================================
                    if (led_cmd_id == CMD_LED_SET_COLOR_LEFT)
                    {
                        set_led(E_LED_LEFT_RED  ,  red  );
                        set_led(E_LED_LEFT_GREEN,  green);
                        set_led(E_LED_LEFT_BLUE ,  blue );
                    }// if it's the left colored LED
                    else
                    {
                        set_led(E_LED_RIGHT_RED  ,  red  );
                        set_led(E_LED_RIGHT_GREEN,  green);
                        set_led(E_LED_RIGHT_BLUE ,  blue );
                    }// if it's the right colored LED
                    
                    break;
                }// CMD_LED_SET_COLOR_LEFT
                
                case CMD_LED_SET_WHITE:
                {
                    if ((param_str = strchr(param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action"); 
                        return FALSE;
                    }// If we can't find the next ',' character
                    
                    if ((param_str = strchr(++param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED index color"); 
                        return FALSE;
                    }// If we can't find the next ':' character

                    // ========================================================
                    // parse index
                    // ========================================================
                    if ((param_str = strchr(++param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED index"); 
                        return FALSE;
                    }// If we can't find the next ':' character

                    if ((next_param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED index"); 
                        return FALSE;
                    }// If we can't find the next ',' character
                    *next_param_str = '\0';
                    next_param_str++;
                    
                    led_index = atoi(param_str);
                    
                    if ((led_index < 0) || (led_index >= NUMBER_OF_WHITE_LEDS))
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "invalid led index"); 
                        return FALSE;
                    }
                    
                    // ========================================================
                    // parse led intensity
                    // ========================================================
                    if ((param_str = strchr(next_param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse green from RGB color"); 
                        return FALSE;
                    }// If we can't find the next ':' character
                    
                    led_intensity = atoi(++param_str);     
                    
                    // ========================================================
                    // execute the command
                    // ========================================================
                    set_led(led_index, led_intensity);
                    break;
                }// CMD_LED_SET_WHITE
                
                case CMD_LED_SET_STRIP:
                {
                    if ((param_str = strchr(param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next ',' character
                    
                    if ((param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next ',' character 
                    
                    if ((param_str = strchr(++param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next ':' character
                    
                    memset(led_vec, 0, sizeof(led_vec));
                    
                    //=========================================================
                    // left red
                    //=========================================================
                    if ((param_str = strchr(++param_str, '\"')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next '"' character
                    param_str++;  
                    led_vec[E_LED_LEFT_RED] = atoi(param_str);
                 
                    //=========================================================
                    // left green
                    //=========================================================
                    if ((param_str = strchr(param_str, '-')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next '-' character
                    param_str++;
                    led_vec[E_LED_LEFT_GREEN] = atoi(param_str);
                    
                    //=========================================================
                    // left blue
                    //=========================================================
                    if ((param_str = strchr(param_str, '-')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next '-' character
                    param_str++;
                    led_vec[E_LED_LEFT_BLUE] = atoi(param_str);
                    
                    //=========================================================
                    // All whites
                    //=========================================================                    
                    for(i = 0; i < NUMBER_OF_WHITE_LEDS; i++)
                    {
                        if ((param_str = strchr(param_str, '-')) == NULL)
                        {
                            send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                            return FALSE;
                        }// If we can't find the next '-' character
                        param_str++;
                        led_vec[i] = atoi(param_str);
                    }// for all the white LEDS
                    
                    //=========================================================
                    // right red
                    //=========================================================
                    if ((param_str = strchr(param_str, '-')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next '-' character
                    param_str++;
                    led_vec[E_LED_RIGHT_RED] = atoi(param_str);
                    
                    //=========================================================
                    // right green
                    //=========================================================
                    if ((param_str = strchr(param_str, '-')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next '-' character
                    param_str++;
                    led_vec[E_LED_RIGHT_GREEN] = atoi(param_str);
                    
                    //=========================================================
                    // right blue
                    //=========================================================
                    if ((param_str = strchr(param_str, '-')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED strip command"); 
                        return FALSE;
                    }// If we can't find the next '-' character
                    param_str++;
                    led_vec[E_LED_RIGHT_BLUE] = atoi(param_str);
                    
                    set_led_strip(led_vec);
                    break;
                }// CMD_LED_SET_STRIP
                
                case CMD_LED_UNKNOWN:
                default             :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported led action"); 
                    return FALSE;
                }// CMD_LED_UNKNOWN, default
            }// switch on the led command
            
            break;
        }// CMD_LED
        
        case CMD_SENSOR:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported sensor action"); 
                return FALSE;
            }// If we can't find the next ',' character
            
            if ((param_str = strchr(++param_str, ':')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported sensor action"); 
                return FALSE;
            }// If we can't find the next ':' character
            param_str++;
            
            switch (sensor_cmd_id = get_sensor_cmd_id_from_cmd_string(param_str))
            {
                case CMD_SENSOR_DISABLE:
                {
                    set_is_sensor_telemetry_enable(FALSE);
                    break;
                }// CMD_TRACKER_DISABLE      
                
                case CMD_SENSOR_ENABLE:
                {
                    set_is_sensor_telemetry_enable(TRUE);
                    break;
                }// CMD_TRACKER_ENABLE
                
                case CMD_SENSOR_UNKNOWN:
                default                 :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported sensor action"); 
                    return FALSE;
                }// CMD_TRACKER_UNKNOWN, default
            }// switch in the tracker action
            break;
        }// CMD_SENSOR
        
       case CMD_TELEMETRY:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported telemetry action"); 
                return FALSE;
            }// If we can't find the next ',' character
            
            if ((param_str = strchr(++param_str, ':')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported telemetry action"); 
                return FALSE;
            }// If we can't find the next ':' character
            param_str++;
            
            switch (telemetry_cmd_id = get_telemetry_cmd_id_from_cmd_string(param_str))
            {
                case CMD_TELEMETRY_DISABLE:
                {
                    set_is_telemetry_enable(FALSE);
                    break;
                }// CMD_TRACKER_DISABLE      
                
                case CMD_TELEMETRY_ENABLE:
                {
                    set_is_telemetry_enable(TRUE);
                    break;
                }// CMD_TRACKER_ENABLE
                
                case CMD_TELEMETRY_UNKNOWN:
                default                   :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported telemetry action"); 
                    return FALSE;
                }// CMD_TRACKER_UNKNOWN, default
            }// switch in the tracker action
            break;
        }// CMD_TELEMETRY

        case CMD_TRACKER:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action"); 
                return FALSE;
            }// If we can't find the next ',' character
            
            if ((param_str = strchr(++param_str, ':')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action"); 
                return FALSE;
            }// If we can't find the next ':' character
            param_str++;
            
            switch (tracker_cmd_id = get_tracker_cmd_id_from_cmd_string(param_str))
            {
                case CMD_TRACKER_DISABLE:
                {
                    set_is_tracker_enable(FALSE);
                    break;
                }// CMD_TRACKER_DISABLE      
                
                case CMD_TRACKER_ENABLE:
                {
                    set_is_tracker_enable(TRUE);
                    break;
                }// CMD_TRACKER_ENABLE
                
                case CMD_TRACKER_SET_BUTTON:
                {
                    if ((param_str = strchr(param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action"); 
                        return FALSE;
                    }// If we can't find the next ',' character
            
                    if ((param_str = strchr(++param_str, ':')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action"); 
                        return FALSE;
                    }// If we can't find the next ':' character
                    param_str++;
                    set_number_of_buttons (atoi(param_str));
                    break;
                }// CMD_TRACKER_SET_BUTTON
                
                case CMD_TRACKER_UNKNOWN:
                default                 :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action"); 
                    return FALSE;
                }// CMD_TRACKER_UNKNOWN, default
            }// switch in the tracker action
            break;
        }// CMD_TRACKER
        
        case CMD_VERSION:
        {
            send_alarm_telemetry(ALARM_INFO, "version", get_software_version());
            break;
        }// CMD_VERSION
        
        case CMD_UNKNOWN:
        default         :
        {
            send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported command");
            return FALSE;
        }// default
    }// switch cmd id
    
    return TRUE; // the command succeed 
}// parse_and_execute_command

//=============================================================================
//============================================ send_bootloader_config_telemetry
//=============================================================================
void send_bootloader_config_telemetry(void)
{
    // FIXME!
}// send_bootloader_config_telemetry

//=============================================================================
//======================================================== send_alarm_telemetry
//=============================================================================
void send_alarm_telemetry(t_enum_alarm_type alarm_type, const char* source, const char* description)
{
    const char* alarm_string[ALARM_UNKNOWN] = {"log", "info", "warning", "error", "fatal"};
 
    memset(telemetry, '\0', sizeof(telemetry));
    
    strcpy(telemetry, "{\"event\":\"alarm\", \"type\":\"");
    strcat(telemetry, alarm_string[alarm_type]);
    strcat(telemetry, "\", \"source\":\"");
    strcat(telemetry, source);
    strcat(telemetry, "\", \"description\":\"");
    strcat(telemetry, description);
    strcat(telemetry, "\"}");
    
    send_telemetry(telemetry);
}// send_alarm_telemetry

//=============================================================================
//==================================================== send_animation_telemetry
//=============================================================================
void send_animation_telemetry(t_enum_animation animation_index, const char* action)
{
    t_struct_animation animation;
 
    get_animation(animation_index, &animation);
    
    memset(telemetry, '\0', sizeof(telemetry));
    strcpy(telemetry, "{\"event\":\"animation\", \"name\":\"");
    strcat(telemetry, animation.description);
    strcat(telemetry, "\", \"action\":\"");
    strcat(telemetry, action);
    strcat(telemetry, "\"}");
    
    send_telemetry(telemetry);
}// send_animation_telemetry

//=============================================================================
//===================================================== send_telemetry_capsense
//=============================================================================
void send_telemetry_capsense(void)
{
    uint16 i;
    uint16 nb_sensors = CapSense_GET_SENSOR_COUNT(0);
   
    for (i = 0; i < nb_sensors; i++)
    {
        memset(telemetry, '\0', sizeof(telemetry));
        strcpy(telemetry, "{\"event\":\"sensor\", \"id\":");
        strcat(telemetry, itoa(i));
        strcat(telemetry, ", \"raw\":");
        strcat(telemetry, itoa(CapSense_dsRam.snsList.linearslider0[i].raw[0]));
        strcat(telemetry, ", \"baseline\":");
        strcat(telemetry, itoa(CapSense_dsRam.snsList.linearslider0[i].bsln[0]));
        strcat(telemetry, ", \"bucket baseline\":");
        strcat(telemetry, itoa(CapSense_dsRam.snsList.linearslider0[i].bslnExt[0]));
        strcat(telemetry, ", \"diff\":");
        strcat(telemetry, itoa(CapSense_dsRam.snsList.linearslider0[i].diff));
        strcat(telemetry, ", \"baseline reset\":");
        strcat(telemetry, itoa(CapSense_dsRam.snsList.linearslider0[i].negBslnRstCnt[0]));
        strcat(telemetry, ", \"IDAC\":");
        strcat(telemetry, itoa(CapSense_dsRam.snsList.linearslider0[i].idacComp[0]));
        strcat(telemetry, "\"}");
        send_telemetry(telemetry);
    }// for all the sensors
}// send_telemetry_capsense

//=============================================================================
//======================================================= send_button_telemetry
//=============================================================================
void send_button_telemetry(int button, const char* action, t_enum_direction finger_direct, uint16 actual_raw_pos, uint16 previous_raw_pos)
{
    int           nb_touched       =  0;
    long long int clock_time       = (get_timer_interrrupt_count() << 16) + Timer_1_ReadCounter();
    char          str_direction[2] = {finger_direct, '\0'};
    uint16        max_sensor       = 0;
    char          sensors_vec[64];
    uint16        i;

    memset(telemetry  , '\0', sizeof(telemetry  ));
    memset(sensors_vec, '\0', sizeof(sensors_vec));
    
    max_sensor = min(CapSense_GET_SENSOR_COUNT(0), sizeof(sensors_vec) - 1);
         
    for (i = 0; i < max_sensor; i++)
    {
        if (CapSense_IsSensorActive(0, i))
        {
            nb_touched++;
            sensors_vec[i] = '1';
        }
        else
        {
             sensors_vec[i] = '0';  
        }
    }// for all the sensors;
    
    strcpy(telemetry, "{\"event\":\"button\", \"id\":");
    strcat(telemetry, itoa(button));
#if USE_ALL_BUTTON_TELE     
    strcat(telemetry, ", \"sensor count\":");
    strcat(telemetry, itoa(nb_touched));
    strcat(telemetry, ", \"clock\":");
    strcat(telemetry, itoa(clock_time));
    strcat(telemetry, ", \"direction\":\"");
    strcat(telemetry, str_direction);
    strcat(telemetry, "\", \"sensors\":\"");
    strcat(telemetry, sensors_vec);
    strcat(telemetry, "\", \"actual raw pos\":");
    strcat(telemetry, itoa(actual_raw_pos));
    strcat(telemetry, ", \"previous raw pos\":");
    strcat(telemetry, itoa(previous_raw_pos));
#endif //  USE_ALL_BUTTON_TELE   
    strcat(telemetry, ", \"action\":\"");
    strcat(telemetry, action);
    strcat(telemetry, "\"}");
    
    send_telemetry(telemetry);
    
    if (get_is_sensor_telemetry_enable())
    {
        send_telemetry_capsense();      
    }
}// send_button_telemetry

//=============================================================================
//================================================== send_push_button_telemetry
//=============================================================================
void send_push_button_telemetry(uint8 capsense_id, char* button_name, BOOL is_button_down)
{
    memset(telemetry, '\0', sizeof(telemetry));
    
    strcat(telemetry, "{\"event\":\"push button\", \"id\":");
    strcat(telemetry, itoa(capsense_id));
    strcat(telemetry, ", \"name\":\"");
    strcat(telemetry, button_name);
    strcat(telemetry, "\", \"state\":\"");
    strcat(telemetry, is_button_down ? "down" : "up"); 
    strcat(telemetry, "\"}");
    
    send_telemetry(telemetry);
}// send_push_button_telemetry

//=============================================================================
//=============================================== send_flash_geometry_telemetry
//=============================================================================
void send_flash_geometry_telemetry(uint32 flash_checksum, uint32 flash_first_row, uint32 flash_last_row, uint32 flash_size)
{
    memset(telemetry, '\0', sizeof(telemetry));
    strcpy(telemetry, "{\"event\":\"flash geometry\", \"checksum\":");
    strcat(telemetry, itoa(flash_checksum));
    strcat(telemetry, ", \"first row\":");
    strcat(telemetry, itoa(flash_first_row));
    strcat(telemetry, ", \"last row\":");
    strcat(telemetry, itoa(flash_last_row));
    strcat(telemetry, ", \"size\":");
    strcat(telemetry, itoa(flash_size));
    strcat(telemetry, "}");
    
    send_telemetry(telemetry);
}// send_flash_geometry_telemetry

//=============================================================================
//============================================================== send_telemetry
//=============================================================================
void send_telemetry(const char *telemetry)
{
    if (get_is_telemetry_enable())
    {
        CapSense_ISR_Disable();
        CommsSendData(telemetry);
        CapSense_ISR_Enable();
    }// If the telemetry are enable
}// send_button_telemetry

//=============================================================================
//===================================================== get_is_telemetry_enable
//=============================================================================
BOOL get_is_telemetry_enable()
{
    return is_telemetry_enable;
}// get_is_telemetry_enable

//=============================================================================
//===================================================== set_is_telemetry_enable
//=============================================================================
void set_is_telemetry_enable(BOOL the_is_telemetry_enable)
{
    is_telemetry_enable = the_is_telemetry_enable;
}// set_is_telemetry_enable

//=============================================================================
//============================================== get_is_sensor_telemetry_enable
//=============================================================================
BOOL get_is_sensor_telemetry_enable()
{
    return is_sensor_telemetry_enable;
}// get_is_sensor_telemetry_enable

//=============================================================================
//============================================== set_is_sensor_telemetry_enable
//=============================================================================
void set_is_sensor_telemetry_enable(BOOL the_is_sensor_telemetry_enable)
{
    is_sensor_telemetry_enable = the_is_sensor_telemetry_enable;
}// set_is_sensor_telemetry_enable

/* [] END OF FILE */
