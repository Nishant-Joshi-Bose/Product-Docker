#include <project.h>

#define VERSION                "0.01"
#define MAX_LEDS               24
#define NUMBER_OF_SEGMENTS     10
#define NUMBER_OF_WHITE_LEDS   15
#define NUMBER_OF_COLORED_LEDS 6
#define SLIDER_RESOLUTION      100
#define TOTAL_NUMBER_OF_LEDS   (NUMBER_OF_WHITE_LEDS + NUMBER_OF_WHITE_LEDS)
#define SLIDER_BUCKETS         (SLIDER_RESOLUTION/NUMBER_OF_SEGMENTS)
#define LED_BUCKETS            (SLIDER_RESOLUTION/NUMBER_OF_WHITE_LEDS)
#define INVALID_CAP_POS        0xFFFF
#define MAX_INTENSITY          0x0FFF //0x00FF // FIXME! 0x0FFF to save the battery
#define MAX_COLOR              0x0FFF
#define START_OF_CMD           '{'
#define END_OF_CMD             '}'
#define USE_UART               1
#define USE_TUNER              0
#define INVALID_LED_POS        ((uint16)UINT_MAX)
#define INT_DIGITS             11 // enough for 32 bits int

#ifndef BOOL
#define BOOL uint8
#endif // BOOL

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif // TRUE

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#endif // min

typedef struct struct_rgb_color {
    char8 red  ;
    char8 green;
    char8 blue ;
}t_struct_rgb_color;

typedef enum enum_primaty_color
{
    E_RED = 0,
    E_GREEN  ,
    E_BLUE
}t_enum_primary_color;

typedef enum enum_color_led_pos
{
    E_LED_RIGHT_RED = 18,
    E_LED_RIGHT_GREEN   ,
    E_LED_RIGHT_BLUE    ,
    E_LED_LEFT_RED      ,
    E_LED_LEFT_GREEN    ,
    E_LED_LEFT_BLUE
}t_enum_color_led_pos;

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
    CMD_ANIMATION,
    CMD_CAPSENSE ,
    CMD_LED      ,
    CMD_HELP     ,
    CMD_TRACKER  ,
    CMD_VERSION  ,
    CMD_UNKNOWN  // keep it last, not a command
}t_enum_command;

typedef enum enum_anim_command
{
    CMD_ANIM_AUTO = 0,
    CMD_ANIM_MANUAL  ,
    CMD_ANIM_STOP    ,
    CMD_ANIM_RESUME  ,
    CMD_ANIM_UNKNOWN  // keep it last
}t_enum_anim_command;

typedef enum enum_tracker_command
{
    CMD_TRACKER_ENABLE = 0,
    CMD_TRACKER_DISABLE   ,
    CMD_TRACKER_UNKNOWN   // keep it last
}t_enum_tracker_command;

typedef enum enum_led_command
{
    CMD_LED_CLEAR = 0      ,
    CMD_LED_SET_COLOR_LEFT ,
    CMD_LED_SET_COLOR_RIGHT,
    CMD_LED_SET_WHITE      ,
    CMD_LED_UNKNOWN
}t_enum_led_command;

typedef enum enum_animation
{
    ANIM_LEFT_RIGHT = 0  ,
    ANIM_INTENSITY       ,
    ANIM_2_LEDS          ,
    ANIM_RED_INTENSITY   ,
    ANIM_GREEN_INTENSITY ,
    ANIM_BLUE_INTENSITY  ,
    ANIM_COLORS_INTENSITY,
    ANIM_RAINBOW_COLORS  ,
    ANIM_FIX_LED_0       ,
    ANIM_FIX_LED_14      ,
    ANIM_UNKNOWN         // keep it last, not an animation
}t_enum_animation;

typedef struct struct_command
{
    uint16      id  ;
    const char* name;
    const char* description;
}t_struct_command;

typedef void (*t_callback_anim) (void);

typedef struct struct_animation
{
    t_enum_animation id         ;
    t_callback_anim  callback   ;
    const char*      description;
}struct_animation_t;

typedef struct tlc_5947_led_driver
{
    uint16 led0 :12;
    uint16 led1 :12;
    uint16 led2 :12;
    uint16 led3 :12;
    uint16 led4 :12;
    uint16 led5 :12;
    uint16 led6 :12;
    uint16 led7 :12;
    uint16 led8 :12;
    uint16 led9 :12;
    uint16 led10:12;
    uint16 led11:12;
    uint16 led12:12;
    uint16 led13:12;
    uint16 led14:12;
    uint16 led15:12;
    uint16 led16:12;
    uint16 led17:12;
    uint16 led18:12;
    uint16 led19:12;
    uint16 led20:12;
    uint16 led21:12;
    uint16 led22:12;
    uint16 led23:12;
}t_tlc_5947_led_driver;

void display_state          (void);
void animate_left_right     (void);
void animate_intensity      (void);
void animate_2_leds         (void);
void animate_random         (void);
void animate_red_intensity  (void);
void animate_green_intensity(void);
void animate_blue_intensity (void);
void animate_all_colors     (void);
void animate_rainbow_colors (void);
void animate_fix_led_0      (void);
void animate_fix_led_14     (void);

void                   animate_color_intensity           (t_enum_primary_color color);
void                   set_led                           (char led, uint16 intensity);
void                   set_all_leds_intensity            (uint16 intensity);
void                   clear_all_leds                    (void);
void                   send_telemetry                    (const char* telemetry);
void                   send_animation_telemetry          (int animation_index, const char* action);
void                   send_button_telemetry             (int button         , const char* action);
void                   send_alarm_telemetry              (t_enum_alarm_type alarm_type, const char* source, const char* description);
BOOL                   parse_and_execute_command         (char* command_str);
t_enum_command         get_cmd_id_from_cmd_string        (const char* command_str);
t_enum_anim_command    get_anim_cmd_id_from_cmd_string   (const char* cmd_str);
t_enum_animation       get_anim_id_from_cmd_string       (const char* cmd_str);
t_enum_tracker_command get_tracker_cmd_id_from_cmd_string(const char* cmd_str);
t_enum_led_command     get_led_cmd_id_from_cmd_string    (const char* cmd_str);

static t_tlc_5947_led_driver led_buffer;
static BOOL                  stop_anim                          = FALSE;
static BOOL                  auto_anim                          = TRUE;
static BOOL                  tracker_enable                     = TRUE;
static int                   current_callback_anim              = ANIM_LEFT_RIGHT;
static uint16                cap_cur_pos                        = INVALID_CAP_POS;
static uint16                cap_old_pos                        = INVALID_CAP_POS;
static uint16                led_pos                            = INVALID_LED_POS;

static struct_animation_t    vec_call_back_anim  [ANIM_UNKNOWN]       ={{ANIM_LEFT_RIGHT        , animate_left_right     , "left-right"                        },
                                                                        {ANIM_INTENSITY         , animate_intensity      , "white intensity"                   },
                                                                        {ANIM_2_LEDS            , animate_2_leds         , "2 leds"                            },
                                                                        {ANIM_RED_INTENSITY     , animate_red_intensity  , "red intensity"                     },
                                                                        {ANIM_GREEN_INTENSITY   , animate_green_intensity, "green intensity"                   },
                                                                        {ANIM_BLUE_INTENSITY    , animate_blue_intensity , "blue intensity"                    },
                                                                        {ANIM_COLORS_INTENSITY  , animate_all_colors     , "all colors"                        },
                                                                        {ANIM_RAINBOW_COLORS    , animate_rainbow_colors , "rainbow colors"                    },
                                                                        {ANIM_FIX_LED_0         , animate_fix_led_0      , "select led 0"                      },
                                                                        {ANIM_FIX_LED_14        , animate_fix_led_14     , "select led 14"                     }};
static t_struct_command      vec_anim_commands   [CMD_ANIM_UNKNOWN]   ={{CMD_ANIM_AUTO          , "auto"                 , "set animation in automatic mode"   },
                                                                        {CMD_ANIM_MANUAL        , "manual"               , "set animation in manual mode"      },
                                                                        {CMD_ANIM_STOP          , "stop"                 , "stop animation"                    },
                                                                        {CMD_ANIM_RESUME        , "resume"               , "resume animation"                  }};
static t_struct_command      vec_tracker_commands[CMD_TRACKER_UNKNOWN]={{CMD_TRACKER_DISABLE    , "disable"              , "disable finger tracker on LEDs"    },
                                                                        {CMD_TRACKER_ENABLE     , "enable"               , "enable finger tracker on LEDs"     }};
static t_struct_command      vec_led_commands    [CMD_LED_UNKNOWN]    ={{CMD_LED_CLEAR          , "clear"                , "clear all leds"                    },
                                                                        {CMD_LED_SET_COLOR_LEFT , "set color left"       , "set the color of the left LED"     },
                                                                        {CMD_LED_SET_COLOR_RIGHT, "set color right"      , "set the color of the right LED"    },
                                                                        {CMD_LED_SET_WHITE      , "set white"            , "set the white LED intensity"       }};
static t_struct_command      vec_commands        [CMD_UNKNOWN]        ={{CMD_ANIMATION          , "animation"            , "set the animation parameters"      },
                                                                        {CMD_CAPSENSE           , "capsense"             , "set/get the capsense configuration"},
                                                                        {CMD_HELP               , "help"                 , ""                                  },
                                                                        {CMD_LED                , "led"                  , "set led(s)"                        },
                                                                        {CMD_TRACKER            , "tracker"              , "set/get the tracker configuration" },
                                                                        {CMD_VERSION            , "version"              , "get the actual version"            }};

char* itoa(int i)
{
    static char buf[INT_DIGITS + 2]; // Room for INT_DIGITS digits, - and '\0'
    char* p = buf + INT_DIGITS + 1;  // points to terminating '\0'

    if (i >= 0) {
        do
        {
            *--p  = '0' + (i % 10);
            i    /= 10;
        } while (i != 0);
        return p;
    }
    else
    {			/* i < 0 */
        do {
            *--p  = '0' - (i % 10);
            i    /= 10;
        } while (i != 0);
        *--p = '-';
    }
    return p;
}// itoa

int atoi(register char* string)
{
    register int          result = 0;
    register unsigned int digit  = 0;
    int                   sign   = 0;

    while (isspace(*string))
    {
	    string += 1;
    }

    if (*string == '-')
    {
    	sign    = 1;
    	string += 1;
    }
    else
    {
    	sign = 0;
    	if (*string == '+')
        {
    	    string += 1;
    	}
    }

    for ( ; ; string += 1)
    {
    	digit = *string - '0';
    	if (digit > 9)
        {
    	    break;
    	}
    	result = (10*result) + digit;
    }

    if (sign)
    {
	    return -result;
    }

    return result;
}// atoi

int main()
{
    BOOL  cap_sense_is_untouched  = TRUE;
    BOOL  cap_sense_was_untouched = TRUE;
    char  uart_char_in            = 0;
    int   receive_pos             = 0;
    char* command                 = NULL;
    char  receive_string[512];

    SPIM_Start ( );
    Blank_Write(0);
    Latch_Write(0);
    CyDelayUs  (1);

    CyGlobalIntEnable; // enable global interrupts before starting capsense and I2C blocks

    clear_all_leds();

#if USE_UART
    UART_1_Start();
#else
    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(sizeof(CapSense_dsRam), sizeof(CapSense_dsRam), (uint8 *)&CapSense_dsRam);
#endif // USE_UART

    CapSense_Start         ();
    CapSense_ScanAllWidgets();

    memset(receive_string, '\0', sizeof(receive_string));

    for(;;)
    {
        // process data for previous scan and initiate new scan only when the capsense hardware is idle
        if(CapSense_NOT_BUSY == CapSense_IsBusy())
        {
            CapSense_ProcessAllWidgets();// Process data for all widgets

            cap_cur_pos            = CapSense_GetCentroidPos(0);
            cap_sense_is_untouched = ((cap_cur_pos == INVALID_CAP_POS) && (cap_old_pos == INVALID_CAP_POS));

            if (cap_sense_is_untouched)
            {
                if (! cap_sense_was_untouched)
                {
                    if (auto_anim)
                    {
                        send_button_telemetry(led_pos, "up");
                        current_callback_anim++;
                        current_callback_anim %= ANIM_UNKNOWN;
                        led_pos                = INVALID_LED_POS;
                        clear_all_leds();
                        send_animation_telemetry(current_callback_anim, "starting");
                    }
                }
                if (! stop_anim)
                {
                    vec_call_back_anim[current_callback_anim].callback();
                }
            }// if capsense is untouched
            else
            {
                if (cap_sense_was_untouched)
                {
                    clear_all_leds();
                    send_button_telemetry   (led_pos, "up");
                    send_animation_telemetry(current_callback_anim, "stopping");
                }
                display_state ();
            }// else, capsense is touched
            cap_sense_was_untouched = cap_sense_is_untouched;

#if USE_UART
            // get, parse and execute the commands
            uart_char_in = UART_1_UartGetChar();
            if((uart_char_in >= 0x20) && (uart_char_in <= 0x7F))
            {
                receive_string[receive_pos++] = uart_char_in;
                if ((uart_char_in == END_OF_CMD) || (receive_pos == sizeof(receive_string)))
                {
                    if ((receive_string[0] != START_OF_CMD) || (receive_string[strlen(receive_string) - 1] != END_OF_CMD))
                    {
                        send_alarm_telemetry(ALARM_WARNING, receive_string, "invalid start-end character(s)");
                    }// if the command is not well formed
                    else
                    {
                        receive_string[strlen(receive_string) - 1] = '\0';
                        if (parse_and_execute_command(&(receive_string[1])) == FALSE)
                        {
                            send_alarm_telemetry(ALARM_WARNING, command, "failed to execute the command");
                        }// If we failed to execute the command
                        else
                        {
                            send_alarm_telemetry(ALARM_LOG, command, "executed");
                        }
                    }// else, the command seems well formed

                    memset (receive_string, '\0', sizeof(receive_string));
                    receive_pos = 0;
                }// If we detect the end of the command
            }// If we got a character
#else
            // FIXME: use I2C
#endif // USE_UART

#if USE_TUNER
            CapSense_RunTuner(); // sync capsense parameters via tuner before the beginning of new capsense scan
#endif // USE_TUNER

            CapSense_ScanAllWidgets();
        }// cap sense not busy
    }// for ever
}// main

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

t_enum_animation get_anim_id_from_cmd_string(const char* cmd_str)
{
    int i;

    for (i = 0; i < ANIM_UNKNOWN; i++)
    {
        if (strstr(cmd_str, vec_call_back_anim[i].description))
        {
            return vec_call_back_anim[i].id;
        }
    }// for all the commands

    return ANIM_UNKNOWN;
}// get_anim_cmd_id_from_cmd_string

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

BOOL parse_and_execute_command(char* cmd_str_and_param)
{
    t_enum_command          cmd_id         = CMD_UNKNOWN;
    t_enum_anim_command     anim_cmd_id    = CMD_ANIM_UNKNOWN;
    t_enum_animation        anim_id        = ANIM_UNKNOWN;
    t_enum_led_command      led_cmd_id     = CMD_LED_UNKNOWN;
    t_enum_tracker_command  tracker_cmd_id = CMD_TRACKER_UNKNOWN;
    char*                   param_str      = strchr(cmd_str_and_param, ',');
    char*                   next_param_str = NULL;
    int                     red            = 0;
    int                     green          = 0;
    int                     blue           = 0;
    int                     led_index      = 0;
    int                     led_intensity  = 0;
    int                     i;

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
                send_alarm_telemetry(ALARM_LOG, "animation", vec_call_back_anim[i].description);
            }// for all the commands

            break;
        }// CMD_HELP

        case CMD_ANIMATION:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action");
                return FALSE;
            }// If we can't find the next comma

            if ((param_str = strchr(++param_str, '=')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action");
                return FALSE;
            }// If we can't find the next equal sign
            param_str++;

            switch (anim_cmd_id = get_anim_cmd_id_from_cmd_string(param_str))
            {
                case CMD_ANIM_AUTO:
                {
                    auto_anim = TRUE;
                    break;
                }// CMD_ANIM_AUTO

                case CMD_ANIM_MANUAL:
                {
                     auto_anim = FALSE;
                    break;
                }// CMD_ANIM_MANUAL

                case CMD_ANIM_STOP:
                {
                    stop_anim = TRUE;
                    break;
                }// CMD_ANIM_STOP

                case CMD_ANIM_RESUME:
                {
                    stop_anim = FALSE;
                    break;
                }// CMD_ANIM_RESUME

                case CMD_ANIM_UNKNOWN:
                default              :
                {
                    if ((anim_id = get_anim_id_from_cmd_string(param_str)) != ANIM_UNKNOWN)
                    {
                        current_callback_anim = anim_id;
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

        case CMD_LED:
        {
            switch (led_cmd_id = get_led_cmd_id_from_cmd_string(param_str))
            {
                case CMD_LED_CLEAR:
                {
                    clear_all_leds();
                    break;
                }// CMD_LED_CLEAR

                case CMD_LED_SET_COLOR_LEFT:
                case CMD_LED_SET_COLOR_RIGHT:
                {
                    if ((param_str = strchr(param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported animation action");
                        return FALSE;
                    }// If we can't find the next comma

                    if ((param_str = strchr(++param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse red from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign

                    // ========================================================
                    // parse red
                    // ========================================================
                    if ((param_str = strchr(++param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse red from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign

                    if ((next_param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse red from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign
                    *next_param_str = '\0';
                    next_param_str++;

                    red = atoi(param_str);

                    // ========================================================
                    // parse green
                    // ========================================================
                    if ((param_str = strchr(next_param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse green from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign

                    if ((next_param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parsed green from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign
                    *next_param_str = '\0';
                    next_param_str++;

                    green = atoi(param_str);

                    // ========================================================
                    // parse blue
                    // ========================================================
                    if ((param_str = strchr(next_param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse green from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign

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
                    }// If we can't find the next comma

                    if ((param_str = strchr(++param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED index color");
                        return FALSE;
                    }// If we can't find the next equal sign

                    // ========================================================
                    // parse index
                    // ========================================================
                    if ((param_str = strchr(++param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED index");
                        return FALSE;
                    }// If we can't find the next equal sign

                    if ((next_param_str = strchr(++param_str, ',')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse LED index");
                        return FALSE;
                    }// If we can't find the next equal sign
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
                    if ((param_str = strchr(next_param_str, '=')) == NULL)
                    {
                        send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "can't parse green from RGB color");
                        return FALSE;
                    }// If we can't find the next equal sign

                    led_intensity = atoi(++param_str);

                    // ========================================================
                    // execute the command
                    // ========================================================
                    set_led(led_index, led_intensity);
                    break;
                }// CMD_LED_SET_WHITE

                case CMD_LED_UNKNOWN:
                default             :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported led action");
                    return FALSE;
                }// CMD_LED_UNKNOWN, default
            }// switch on the led command

            break;
        }// CMD_LED

        case CMD_TRACKER:
        {
            if ((param_str = strchr(param_str, ',')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action");
                return FALSE;
            }// If we can't find the next comma

            if ((param_str = strchr(++param_str, '=')) == NULL)
            {
                send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action");
                return FALSE;
            }// If we can't find the next equal sign
            param_str++;

            switch (tracker_cmd_id = get_tracker_cmd_id_from_cmd_string(param_str))
            {
                case CMD_TRACKER_DISABLE:
                {
                    tracker_enable = FALSE;
                    break;
                }// CMD_TRACKER_DISABLE

                case CMD_TRACKER_ENABLE:
                {
                    tracker_enable = TRUE;
                    break;
                }// CMD_TRACKER_ENABLE

                case CMD_TRACKER_UNKNOWN:
                default                 :
                {
                    send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported tracker action");
                    return FALSE;
                }// CMD_TRACKER_UNKNOWN, default
            }// switch in the tracker action
            break;
        }// CMD_SET_TRACKER

        case CMD_VERSION:
        {
            send_alarm_telemetry(ALARM_INFO, "version", VERSION);
            break;
        }// CMD_VERSION

        case CMD_UNKNOWN:
        default         :
        {
            send_alarm_telemetry(ALARM_WARNING, cmd_str_and_param, "unsupported command");
            return FALSE;
        }// default
    }// switch cmd id

#if 0
    if (strstr(receive_string, "switch anim"))
                    {
                        current_callback_anim++;
                        current_callback_anim %= NB_CALLBACK_ANIM;
                        led_pos                = INVALID_LED_POS;
                        clear_all_leds();
                        send_animation_telemetry(current_callback_anim, "switching");
                    }
                    else if (strstr(receive_string, "suspend anim"))
                    {
                        stop_anim = TRUE;
                        led_pos   = INVALID_LED_POS;
                        clear_all_leds();
                        send_animation_telemetry(current_callback_anim, "suspend");
                    }
                    else if (strstr(receive_string, "resume anim"))
                    {
                        stop_anim = FALSE;
                        led_pos   = INVALID_LED_POS;
                        clear_all_leds();
                        send_animation_telemetry(current_callback_anim, "resumed");
                    }
                    else
                    {
                        send_alarm_telemetry(ALARM_WARNING, receive_string, "unsupported command");
                    }

                }// If we received all the commad string
#endif // 0

    return TRUE; // the command succeed
}// parse_and_execute_command

void send_alarm_telemetry(t_enum_alarm_type alarm_type, const char* source, const char* description)
{
    char        telemetry[256];
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

void send_animation_telemetry(int animation_index, const char* action)
{
    char telemetry[256];

    memset(telemetry, '\0', sizeof(telemetry));

    strcpy(telemetry, "{\"event\":\"animation\", \"name\":\"");
    strcat(telemetry, vec_call_back_anim[animation_index].description);
    strcat(telemetry, "\", \"action\":\"");
    strcat(telemetry, action);
    strcat(telemetry, "\"}");

    send_telemetry(telemetry);
}// send_animation_telemetry

void send_button_telemetry(int button, const char* action)
{
    char telemetry[256];

    memset(telemetry, '\0', sizeof(telemetry));

    strcpy(telemetry, "{\"event\":\"button\", \"id\":");
    strcat(telemetry, itoa(button));
    strcat(telemetry, ", \"action\":\"");
    strcat(telemetry, action);
    strcat(telemetry, "\"}");

    send_telemetry(telemetry);
}// send_button_telemetry

void send_telemetry(const char *telemetry)
{
#ifdef USE_UART
    UART_1_UartPutString(telemetry);
#else
    // FIXME: use I2C
#endif //USE_UART
}// send_button_telemetry

void display_state(void)
{
    int new_led_pos = 0;

    if (cap_cur_pos != cap_old_pos)
    {
        cap_old_pos = cap_cur_pos;
        new_led_pos = cap_cur_pos / LED_BUCKETS;
        if ((cap_cur_pos != 0u) && (new_led_pos != led_pos) && (new_led_pos >= 0) && (new_led_pos <= NUMBER_OF_WHITE_LEDS - 1))
        {
            if (tracker_enable)
            {
                set_led(led_pos    , 0x00         );
                set_led(new_led_pos, MAX_INTENSITY);
            }

            led_pos = new_led_pos;
            send_button_telemetry(led_pos, "position");
         }
    }
}// display_state

void animate_all_colors(void)
{
    static uint16 red             = 0;
    static uint16 green           = 0;
    static uint16 blue            = 0;
    static BOOL   intensity_up    = TRUE;
           uint16 blue_increment  = 64 ;
           uint16 green_increment = 128;
           uint16 red_increment   = 256;

    set_led(E_LED_LEFT_RED   ,  red  );
    set_led(E_LED_LEFT_GREEN ,  green);
    set_led(E_LED_LEFT_BLUE  ,  blue );
    set_led(E_LED_RIGHT_RED  , ~red  );
    set_led(E_LED_RIGHT_GREEN, ~green);
    set_led(E_LED_RIGHT_BLUE , ~blue );

    if (intensity_up)
    {
        if (red >= MAX_COLOR)
        {
            intensity_up = FALSE;
        }
        else if (green >= MAX_COLOR)
        {
           red   += red_increment;
           green  = 0;
           blue   = 0;
        }
        else if (blue >= MAX_COLOR)
        {
            green += green_increment;
            blue   = 0;
        }
        else
        {
            blue += blue_increment;
        }
    }// if the intensity is going up
    else
    {
        if (red == 0)
        {
            intensity_up = TRUE;
        }
        else if (green == 0)
        {
           red   -= red_increment;
           green  = MAX_COLOR;
           blue   = MAX_COLOR;
        }
        else if (blue == 0)
        {
            green -= green_increment;
            blue   = MAX_COLOR;
        }
        else
        {
            blue -= blue_increment;
        }
    }// // else, the intensity is going down
}// animate_all_colors

void animate_fix_led_0(void)
{
    set_led(0, MAX_INTENSITY);
}// animate_fix_led_0

void animate_fix_led_14(void)
{
    set_led(14, MAX_INTENSITY);
}// animate_fix_led_0

void animate_red_intensity  (void)
{
    animate_color_intensity(E_RED);
}// animate_red_intensity

void animate_green_intensity(void)
{
    animate_color_intensity(E_GREEN);
}// animate_green_intensity

void animate_blue_intensity (void)
{
     animate_color_intensity(E_BLUE);
}// animate_blue_intensity

void animate_color_intensity(t_enum_primary_color color)
{
    static uint16 intensity    = 0   ;
    static BOOL   intensity_up = TRUE;
           uint16 increment    = 5   ;

    switch (color)
    {
        case E_RED  : set_led(E_LED_LEFT_RED   , intensity);
                      set_led(E_LED_RIGHT_RED  , intensity);
                      break;
        case E_GREEN: set_led(E_LED_LEFT_GREEN , intensity);
                      set_led(E_LED_RIGHT_GREEN, intensity);
                      break;
        case E_BLUE : set_led(E_LED_LEFT_BLUE  , intensity);
                      set_led(E_LED_RIGHT_BLUE , intensity);
                      break;
    }// switch color

    if (intensity_up)
    {
        intensity += increment;
        if (intensity >= MAX_COLOR)
        {
            intensity_up = FALSE;
        }
    }// if the intensity is going up
    else
    {
        intensity -= increment;
        if (intensity == 0)
        {
            intensity_up = TRUE;
        }
    }
}// animate_color_intensity

void animate_rainbow_colors()
{
    static uint16             cycle         = 0;
    static uint16             color_index   = 0;
    static t_struct_rgb_color vec_rainbow[] = {{148,   0, 211}, // violet
                                               { 75,   0, 130}, // indigo
                                               {  0,   0, 255}, // blue
                                               {  0, 255,   0}, // green
                                               {255, 255,   0}, // yellow
                                               {255, 127,   0}, // orange
                                               {255,   0,   0}, // red
                                               {  0,   0,   0}};
           BOOL               skip_cycle    = cycle % 64;

    cycle++;

    if (skip_cycle)
    {
        return;
    }

    if ((vec_rainbow[color_index].red + vec_rainbow[color_index].green + vec_rainbow[color_index].blue) == 0)
    {
        color_index = 0;
    }

    set_led(E_LED_LEFT_RED   , vec_rainbow[color_index].red  );
    set_led(E_LED_RIGHT_RED  , vec_rainbow[color_index].red  );
    set_led(E_LED_LEFT_GREEN , vec_rainbow[color_index].green);
    set_led(E_LED_RIGHT_GREEN, vec_rainbow[color_index].green);
    set_led(E_LED_LEFT_BLUE  , vec_rainbow[color_index].blue );
    set_led(E_LED_RIGHT_BLUE , vec_rainbow[color_index].blue );
    color_index++;
}// animate_rainbow_colors

void animate_left_right()
{
    static uint16 cycle      = 0;
           BOOL   skip_cycle = cycle % 10;

    cycle++;

    if (skip_cycle)
    {
        return;
    }

    set_led(led_pos, 0x00);

    led_pos++;
    led_pos %= NUMBER_OF_WHITE_LEDS;

    set_led(led_pos, MAX_INTENSITY);
}// animate_left_right

void animate_intensity()
{
    static uint16 intensity    = 0   ;
    static BOOL   intensity_up = TRUE;
           uint16 increment    = 1   ;
           int    i                  ;

    for (i = 0; i < NUMBER_OF_WHITE_LEDS; i++)
    {
        set_led(i, intensity);
    }

    if (intensity_up)
    {
        intensity += increment;
        if (intensity >= 0x0FF)
        {
            intensity_up = FALSE;
        }
    }// if the intensiry is going up
    else
    {
        intensity -= increment;
        if (intensity == 0)
        {
            intensity_up = TRUE;
        }
    }
}// animate_intensity

void animate_2_leds()
{
    static uint16 cycle      = 0;
    static uint16 right_led  = (NUMBER_OF_WHITE_LEDS / 2);
    static uint16 left_led   = (NUMBER_OF_WHITE_LEDS / 2) - 1;
    static BOOL   outward    = TRUE;
           BOOL   skip_cycle = cycle % 5;

    cycle++;

    if (skip_cycle)
    {
        return;
    }

    clear_all_leds();

    set_led(left_led , MAX_INTENSITY);
    set_led(right_led, MAX_INTENSITY);

    if (outward)
    {
        left_led --;
        right_led++;
        outward = left_led; // if left_led == 0, we go inward
    }// if we go outward
    else
    {
        left_led ++;
        right_led--;
        outward = (right_led == (NUMBER_OF_WHITE_LEDS / 2));
    }// we go inward
}// animate_2_leds

void set_led (char led, uint16 intensity)
{
    uint16  set_value = intensity; // min(intensity, MAX_INTENSITY);
    uint16  led_min   = min(led, MAX_LEDS - 1);

    switch (led_min)
    {
        case 23: led_buffer.led0  = set_value;
                 break;
        case 22: led_buffer.led1  = set_value;
                 break;
        case 21: led_buffer.led2  = set_value;
                 break;
        case 20: led_buffer.led3  = set_value;
                 break;
        case 19: led_buffer.led4  = set_value;
                 break;
        case 18: led_buffer.led5  = set_value;
                 break;
        case 17: led_buffer.led6  = set_value;
                 break;
        case 16: led_buffer.led7  = set_value;
                 break;
        case 15: led_buffer.led8  = set_value;
                 break;
        case 14: led_buffer.led9  = set_value;
                 break;
        case 13: led_buffer.led10 = set_value;
                 break;
        case 12: led_buffer.led11 = set_value;
                 break;
        case 11: led_buffer.led12 = set_value;
                 break;
        case 10: led_buffer.led13 = set_value;
                 break;
        case  9: led_buffer.led14 = set_value;
                 break;
        case  8: led_buffer.led15 = set_value;
                 break;
        case  7: led_buffer.led16 = set_value;
                 break;
        case  6: led_buffer.led17 = set_value;
                 break;
        case  5: led_buffer.led18 = set_value;
                 break;
        case  4: led_buffer.led19 = set_value;
                 break;
        case  3: led_buffer.led20 = set_value;
                 break;
        case  2: led_buffer.led21 = set_value;
                 break;
        case  1: led_buffer.led22 = set_value;
                 break;
        case  0: led_buffer.led23 = set_value;
                 break;
        default: return;
    }

    SPIM_SpiUartPutArray((const uint16*)&led_buffer, sizeof(led_buffer) / sizeof(uint16));
    Latch_Write(1);
    CyDelayUs  (1);
    Latch_Write(0);
}// set_led

void set_all_leds_intensity (uint16 intensity)
{
    uint16  set_value = intensity; // min(intensity, MAX_INTENSITY);

    clear_all_leds();
    led_buffer.led0  = set_value;
    led_buffer.led1  = set_value;
    led_buffer.led2  = set_value;
    led_buffer.led3  = set_value;
    led_buffer.led4  = set_value;
    led_buffer.led5  = set_value;
    led_buffer.led6  = set_value;
    led_buffer.led7  = set_value;
    led_buffer.led8  = set_value;
    led_buffer.led9  = set_value;
    led_buffer.led10 = set_value;
    led_buffer.led11 = set_value;
    led_buffer.led12 = set_value;
    led_buffer.led13 = set_value;
    led_buffer.led14 = set_value;
    led_buffer.led15 = set_value;
    led_buffer.led16 = set_value;
    led_buffer.led17 = set_value;
    led_buffer.led18 = set_value;
    led_buffer.led19 = set_value;
    led_buffer.led20 = set_value;
    led_buffer.led21 = set_value;
    led_buffer.led22 = set_value;
    led_buffer.led23 = set_value;

    SPIM_SpiUartPutArray((const uint16*)&led_buffer, sizeof(led_buffer) / sizeof(uint16));
    Latch_Write(1);
    CyDelayUs  (1);
    Latch_Write(0);
}// set_led

void clear_all_leds(void)
{
    memset(&led_buffer, 0x00, sizeof(led_buffer));

    SPIM_SpiUartPutArray((const uint16*)&led_buffer, sizeof(led_buffer) / sizeof(uint16));
    Latch_Write(1);
    CyDelayUs  (1);
    Latch_Write(0);
}// clear_all_leds

/* [] END OF FILE */

