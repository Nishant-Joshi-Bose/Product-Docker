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

#include <math.h>

#include "animation.h"



#include "command_telemetry.h"
#include "util.h"

//=============================================================================
//============================================================ static variables
//=============================================================================
static t_struct_animation vec_call_back_anim[ANIM_UNKNOWN] = {{ANIM_BUTTON_SMOOTH_SLOW   , FALSE, FALSE, animate_button_smooth_slow   , "slow breather"                     },
                                                              {ANIM_BUTTON_SMOOTH_FAST   , FALSE, FALSE, animate_button_smooth_fast   , "fast breather"                     },
                                                              {ANIM_LEFT_RIGHT           , FALSE, FALSE, animate_left_right           , "left-right"                        }, 
                                                              {ANIM_INTENSITY            , FALSE, FALSE, animate_intensity            , "white intensity"                   },
                                                              {ANIM_2_LEDS               , FALSE, FALSE, animate_2_leds               , "2 leds"                            },
                                                              {ANIM_RED_INTENSITY        , FALSE, FALSE, animate_red_intensity        , "red intensity"                     },
                                                              {ANIM_GREEN_INTENSITY      , FALSE, FALSE, animate_green_intensity      , "green intensity"                   },
                                                              {ANIM_BLUE_INTENSITY       , FALSE, FALSE, animate_blue_intensity       , "blue intensity"                    },
                                                              {ANIM_RED_WHITE_INTENSITY  , FALSE, FALSE, animate_red_white_intensity  , "red-white intensity"               },
                                                              {ANIM_GREEN_WHITE_INTENSITY, FALSE, FALSE, animate_green_white_intensity, "green-white  intensity"            },
                                                              {ANIM_BLUE_WHITE_INTENSITY , FALSE, FALSE, animate_blue_white_intensity , "blue-white  intensity"             },   
                                                              {ANIM_COLORS_INTENSITY     , FALSE, FALSE, animate_all_colors           , "all colors"                        },
                                                              {ANIM_RAINBOW_COLORS       , FALSE, FALSE, animate_rainbow_colors       , "rainbow colors"                    },
                                                              {ANIM_FIX_WHITE            , FALSE, FALSE, animate_fix_white            , "4 fix white LEDs at full intensity"}};


#if USE_128_POINTS_FOR_BREATHING
#define SINE_VEC_SIZE 128                                                                 // Indexes    degrees                     
static uint16 sine_vec[SINE_VEC_SIZE] = {0x800,0x864,0x8c8,0x92c,0x98f,0x9f1,0xa52,0xab1, // [000..007] [000..089]
                                         0xb0f,0xb6b,0xbc5,0xc1c,0xc71,0xcc3,0xd12,0xd5f, // [008..015]
                                         0xda7,0xded,0xe2e,0xe6c,0xea6,0xedc,0xf0d,0xf3a, // [016..023]
                                         0xf63,0xf87,0xfa7,0xfc2,0xfd8,0xfe9,0xff5,0xffd, // [024..031] 
                                         0xfff,0xffd,0xff5,0xfe9,0xfd8,0xfc2,0xfa7,0xf87, // [032..039] [090..079]
                                         0xf63,0xf3a,0xf0d,0xedc,0xea6,0xe6c,0xe2e,0xded, // [040..047]
                                         0xda7,0xd5f,0xd12,0xcc3,0xc71,0xc1c,0xbc5,0xb6b, // [048..055] 
                                         0xb0f,0xab1,0xa52,0x9f1,0x98f,0x92c,0x8c8,0x864, // [056..063]
                                         0x800,0x79b,0x737,0x6d3,0x670,0x60e,0x5ad,0x54e, // [064..071] [180..269]
                                         0x4f0,0x494,0x43a,0x3e3,0x38e,0x33c,0x2ed,0x2a0, // [072..079]
                                         0x258,0x212,0x1d1,0x193,0x159,0x123,0x0f2,0x0c5, // [080..087]
                                         0x09c,0x078,0x058,0x03d,0x027,0x016,0x00a,0x002, // [088..095]
                                         0x000,0x002,0x00a,0x016,0x027,0x03d,0x058,0x078, // [096..103] [270..359]
                                         0x09c,0x0c5,0x0f2,0x123,0x159,0x193,0x1d1,0x212, // [104..111]
                                         0x258,0x2a0,0x2ed,0x33c,0x38e,0x3e3,0x43a,0x494, // [112..119]
                                         0x4f0,0x54e,0x5ad,0x60e,0x670,0x6d3,0x737,0x79b};// [120..127]            
#else
#define SINE_VEC_SIZE 256
static uint16 sine_vec[SINE_VEC_SIZE] = {0x800,0x832,0x864,0x896,0x8c8,0x8fa,0x92c,0x95e,
                                         0x98f,0x9c0,0x9f1,0xa22,0xa52,0xa82,0xab1,0xae0,
                                         0xb0f,0xb3d,0xb6b,0xb98,0xbc5,0xbf1,0xc1c,0xc47,
                                         0xc71,0xc9a,0xcc3,0xceb,0xd12,0xd39,0xd5f,0xd83,
                                         0xda7,0xdca,0xded,0xe0e,0xe2e,0xe4e,0xe6c,0xe8a,
                                         0xea6,0xec1,0xedc,0xef5,0xf0d,0xf24,0xf3a,0xf4f,
                                         0xf63,0xf76,0xf87,0xf98,0xfa7,0xfb5,0xfc2,0xfcd,
                                         0xfd8,0xfe1,0xfe9,0xff0,0xff5,0xff9,0xffd,0xffe,
                                         0xfff,0xffe,0xffd,0xff9,0xff5,0xff0,0xfe9,0xfe1,
                                         0xfd8,0xfcd,0xfc2,0xfb5,0xfa7,0xf98,0xf87,0xf76,
                                         0xf63,0xf4f,0xf3a,0xf24,0xf0d,0xef5,0xedc,0xec1,
                                         0xea6,0xe8a,0xe6c,0xe4e,0xe2e,0xe0e,0xded,0xdca,
                                         0xda7,0xd83,0xd5f,0xd39,0xd12,0xceb,0xcc3,0xc9a,
                                         0xc71,0xc47,0xc1c,0xbf1,0xbc5,0xb98,0xb6b,0xb3d,
                                         0xb0f,0xae0,0xab1,0xa82,0xa52,0xa22,0x9f1,0x9c0,
                                         0x98f,0x95e,0x92c,0x8fa,0x8c8,0x896,0x864,0x832,
                                         0x800,0x7cd,0x79b,0x769,0x737,0x705,0x6d3,0x6a1,
                                         0x670,0x63f,0x60e,0x5dd,0x5ad,0x57d,0x54e,0x51f,
                                         0x4f0,0x4c2,0x494,0x467,0x43a,0x40e,0x3e3,0x3b8,
                                         0x38e,0x365,0x33c,0x314,0x2ed,0x2c6,0x2a0,0x27c,
                                         0x258,0x235,0x212,0x1f1,0x1d1,0x1b1,0x193,0x175,
                                         0x159,0x13e,0x123,0x10a,0x0f2,0x0db,0x0c5,0x0b0,
                                         0x09c,0x089,0x078,0x067,0x058,0x04a,0x03d,0x032,
                                         0x027,0x01e,0x016,0x00f,0x00a,0x006,0x002,0x001,
                                         0x000,0x001,0x002,0x006,0x00a,0x00f,0x016,0x01e,
                                         0x027,0x032,0x03d,0x04a,0x058,0x067,0x078,0x089,
                                         0x09c,0x0b0,0x0c5,0x0db,0x0f2,0x10a,0x123,0x13e,
                                         0x159,0x175,0x193,0x1b1,0x1d1,0x1f1,0x212,0x235,
                                         0x258,0x27c,0x2a0,0x2c6,0x2ed,0x314,0x33c,0x365,
                                         0x38e,0x3b8,0x3e3,0x40e,0x43a,0x467,0x494,0x4c2,
                                         0x4f0,0x51f,0x54e,0x57d,0x5ad,0x5dd,0x60e,0x63f,
                                         0x670,0x6a1,0x6d3,0x705,0x737,0x769,0x79b,0x7cd};         
#endif // USE_128_POINTS_FOR_BREATHING
                                               
#define PI_0_INDEX   0                  // 0   degrees
#define PI_90_INDEX  (SINE_VEC_SIZE / 4)// 90  degrees
#define PI_120_INDEX (SINE_VEC_SIZE / 3)// 120 degrees
#define PI_180_INDEX (PI_90_INDEX   * 2)// 180 degrees
#define PI_270_INDEX (PI_90_INDEX   * 3)// 270 degrees

static BOOL             is_auto_anim_rotation_enable = TRUE; // when enable: auto rotation of the lightbar animation
static BOOL             is_anim_on_button_up_enable  = TRUE; // when anable: start the selected lightbar animation on a button up
static t_enum_animation current_callback_anim        = ANIM_BUTTON_SMOOTH_SLOW;

//=============================================================================
//========================================================== animate_all_colors
//=============================================================================
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

//=============================================================================
//=========================================================== animate_fix_white
//=============================================================================
void animate_fix_white(void)
{
    set_led(6, MAX_INTENSITY);
    set_led(7, MAX_INTENSITY);
    set_led(8, MAX_INTENSITY);
    set_led(9, MAX_INTENSITY);
}// animate_fix_led_0

//=============================================================================
//========================================================== animate_fix_led_14
//=============================================================================
void animate_fix_led_14(void)
{
    set_led(14, MAX_INTENSITY);
}// animate_fix_led_0

//=============================================================================
//======================================================= animate_red_intensity
//=============================================================================
void animate_red_intensity  (void)
{
#if FOR_COLIN    
    animate_color_intensity(E_RED);
#else
    set_led(E_LED_LEFT_RED   , MAX_COLOR);
    set_led(E_LED_LEFT_GREEN , 0x00     );
    set_led(E_LED_LEFT_BLUE  , 0x00     );
    set_led(E_LED_RIGHT_RED  , MAX_COLOR);
    set_led(E_LED_RIGHT_GREEN, 0x00     );
    set_led(E_LED_RIGHT_BLUE , 0x00     );
#endif        
}// animate_red_intensity

//=============================================================================
//===================================================== animate_green_intensity
//=============================================================================
void animate_green_intensity(void)
{
#if FOR_COLIN    
    animate_color_intensity(E_GREEN);
#else
    set_led(E_LED_LEFT_RED   , 0x00     );
    set_led(E_LED_LEFT_GREEN , MAX_COLOR);
    set_led(E_LED_LEFT_BLUE  , 0x00     );
    set_led(E_LED_RIGHT_RED  , 0x00     );
    set_led(E_LED_RIGHT_GREEN, MAX_COLOR);
    set_led(E_LED_RIGHT_BLUE , 0x00     );
#endif    
}// animate_green_intensity

//=============================================================================
//====================================================== animate_blue_intensity
//=============================================================================
void animate_blue_intensity (void)
{
#if FOR_COLIN    
    animate_color_intensity(E_BLUE);
#else
    set_led(E_LED_LEFT_RED   , 0x00     );
    set_led(E_LED_LEFT_GREEN , 0x00     );
    set_led(E_LED_LEFT_BLUE  , MAX_COLOR);
    set_led(E_LED_RIGHT_RED  , 0x00     );
    set_led(E_LED_RIGHT_GREEN, 0x00     );
    set_led(E_LED_RIGHT_BLUE , MAX_COLOR);
#endif        
}// animate_blue_intensity

//=============================================================================
//================================================= animate_red_white_intensity
//=============================================================================
void animate_red_white_intensity  (void)
{
    static uint8 intensity_white_index = PI_0_INDEX  ;
    static uint8 intensity_color_index = PI_180_INDEX;
      
    set_all_white_leds(sine_vec[intensity_white_index++]);
    set_led           (E_LED_LEFT_RED, sine_vec[intensity_color_index  ]);
    set_led           (E_LED_LEFT_RED, sine_vec[intensity_color_index++]);
    
#if USE_128_POINTS_FOR_BREATHING
    left_index  %= SINE_VEC_SIZE;
    right_index %= SINE_VEC_SIZE;
#endif // at 256 points, the index value (uint8) will rotate by them self
    
}// animate_red_white_intensity

//=============================================================================
//=============================================== animate_green_white_intensity
//=============================================================================
void animate_green_white_intensity(void)
{
    static uint8 intensity_white_index = PI_0_INDEX  ;
    static uint8 intensity_color_index = PI_180_INDEX;
      
    set_all_white_leds(sine_vec[intensity_white_index++]);
    set_led           (E_LED_LEFT_GREEN, sine_vec[intensity_color_index  ]);
    set_led           (E_LED_LEFT_GREEN, sine_vec[intensity_color_index++]);
    
#if USE_128_POINTS_FOR_BREATHING
    left_index  %= SINE_VEC_SIZE;
    right_index %= SINE_VEC_SIZE;
#endif // at 256 points, the index value (uint8) will rotate by them self
}// animate_green_white_intensity

//=============================================================================
//================================================ animate_blue_white_intensity
//=============================================================================
void animate_blue_white_intensity (void)
{
    static uint8 intensity_white_index = PI_0_INDEX  ;
    static uint8 intensity_color_index = PI_180_INDEX;
      
    set_all_white_leds(sine_vec[intensity_white_index++]);
    set_led           (E_LED_LEFT_BLUE, sine_vec[intensity_color_index  ]);
    set_led           (E_LED_LEFT_BLUE, sine_vec[intensity_color_index++]);
    
#if USE_128_POINTS_FOR_BREATHING
    left_index  %= SINE_VEC_SIZE;
    right_index %= SINE_VEC_SIZE;
#endif // at 256 points, the index value (uint8) will rotate by them self  
}// animate_blue_white_intensity

//=============================================================================
//===================================================== animate_color_intensity
//=============================================================================
void animate_color_intensity(t_enum_primary_color color)
{
    static uint16 intensity    = 0   ;
    static BOOL   intensity_up = TRUE;
           uint16 increment    = 1   ;

    switch (color)
    {
        case E_PRIMARY_COLOR_RED  : set_led(E_LED_LEFT_RED   , intensity);
                                    set_led(E_LED_RIGHT_RED  , intensity);
                                    break;
        case E_PRIMARY_COLOR_GREEN: set_led(E_LED_LEFT_GREEN , intensity);
                                    set_led(E_LED_RIGHT_GREEN, intensity);
                                    break;
        case E_PRIMARY_COLOR_BLUE : set_led(E_LED_LEFT_BLUE  , intensity);
                                    set_led(E_LED_RIGHT_BLUE , intensity);
                                    break;
        default                   : break;
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

//=============================================================================
//====================================================== animate_rainbow_colors
//=============================================================================
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
        
    cycle++;
    if (cycle % 64)
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

//=============================================================================
//========================================================== animate_left_right
//=============================================================================
void animate_left_right()
{
    static uint16 cycle   = 0;
    static uint16 led_pos = 0;
         
    cycle++;
    if (cycle % 13)
    {
        return;
    }

    set_led(led_pos, 0x00);

    led_pos++;
    led_pos %= NUMBER_OF_WHITE_LEDS;

    set_led(led_pos, MAX_INTENSITY);
}// animate_left_right

//=============================================================================
//=========================================================== animate_intensity
//=============================================================================
void animate_intensity()
{
    static uint8 intensity_left_index  = PI_0_INDEX ;
    static uint8 intensity_right_index = PI_180_INDEX;
    static uint8 half_white            = NUMBER_OF_WHITE_LEDS / 2;
    int i;
       
    for (i = 0; i < half_white; i++)
    {
        set_led(i, sine_vec[intensity_left_index]);
    }
    
    for (i = half_white; i < NUMBER_OF_WHITE_LEDS; i++)
    {
        set_led(i, sine_vec[intensity_right_index]);
    }
    
    intensity_left_index++;
    intensity_right_index++;

    set_led(E_LED_LEFT_RED   , MAX_INTENSITY);
    set_led(E_LED_LEFT_GREEN , MAX_INTENSITY);
    set_led(E_LED_RIGHT_RED  , MAX_INTENSITY);
    set_led(E_LED_RIGHT_GREEN, MAX_INTENSITY);
    
#if USE_128_POINTS_FOR_BREATHING
    intensity_left_index  %= SINE_VEC_SIZE;
    intensity_right_index %= SINE_VEC_SIZE;
#endif // at 256 points, the index value (uint8) will rotate by them self  
}// animate_intensity

//=============================================================================
//============================================================== animate_2_leds
//=============================================================================
void animate_2_leds()
{
    static uint16               cycle         = 0;
    static uint16               right_led     = (NUMBER_OF_WHITE_LEDS / 2);
    static uint16               left_led      = (NUMBER_OF_WHITE_LEDS / 2) - 1;
    static BOOL                 outward       = TRUE;
    static t_enum_primary_color primary_color = E_PRIMARY_COLOR_RED;
          
    cycle++;
    if (cycle % 20)
    {
        return;
    }
 
    set_all_white_leds(0x00);

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
        if (outward)
        {
            primary_color++;
            primary_color %= E_PRIMARY_COLOR_NUMBER;
            set_all_colored_leds(0x00);
            switch (primary_color)
            {
                case E_PRIMARY_COLOR_RED  : set_led(E_LED_LEFT_RED   , MAX_INTENSITY);
                                            set_led(E_LED_RIGHT_RED  , MAX_INTENSITY);
                                            break;
                case E_PRIMARY_COLOR_GREEN: set_led(E_LED_LEFT_GREEN , MAX_INTENSITY);
                                            set_led(E_LED_RIGHT_GREEN, MAX_INTENSITY);
                                            break; 
                case E_PRIMARY_COLOR_BLUE : set_led(E_LED_LEFT_BLUE  , MAX_INTENSITY);
                                            set_led(E_LED_RIGHT_BLUE , MAX_INTENSITY);
                                            break;
                default:                    break;
            }
        }
    }// we go inward
}// animate_2_leds

void animate_button_smooth(uint16 breather_skip_cycle)
{
    static uint16 led_up      = TRUE;
    static uint16 left_led    = 1  ;
    static uint16 right_led   = 2   ;
    static uint8  left_index  = PI_0_INDEX  ;
    static uint8  right_index = PI_180_INDEX;
    static uint16 cycle       = 0;
           uint16 tmp         = 0;
    
    cycle++;
    
 #if 1   
    if ((breather_skip_cycle) && (cycle % breather_skip_cycle))
    {
        return;    
    }
 #endif   
    
#if USE_128_POINTS_FOR_BREATHING
    left_index  %= SINE_VEC_SIZE;
    right_index %= SINE_VEC_SIZE;
#endif // at 256 points, the index value (uint8) will rotate by them self

#if 1
    if (led_up)
    {
        if (left_index == PI_270_INDEX) // left to right breathing cycle completed
        {
            if (right_led >= NUMBER_OF_WHITE_LEDS - 1)
            {
                led_up = FALSE; 
            }
            else
            {
                left_led++;
                right_led   = left_led + 1;
                tmp         = left_index;
                left_index  = right_index;
                right_index = tmp;
            }
        }// If the left led is at 0 intensity
    }// led up, left to right
    else
    {
        if (right_index == PI_270_INDEX) // right to left breathing cycle completed
        {
            if (left_led == 1)
            {
                led_up = TRUE;
            }
            else
            {
                right_led--;    
                left_led    = right_led - 1;
                tmp         = left_index;
                left_index  = right_index;
                right_index = tmp;
            }  
        }// If the right led is at 0 intensity
    }// else, led down, right to left
#endif

    set_led_pair(left_led, sine_vec[left_index++], sine_vec[right_index++]); 
}// animate_button_smooth

void animate_button_smooth_slow(void)
{
    animate_button_smooth(/*0x400*/ 0x04);
    
}// animate_button_smooth_slow

void animate_button_smooth_fast(void)
{
     animate_button_smooth(/*0x10*/ 0x00);
}// animate_button_smooth_fast

//=============================================================================
//=============================================================== get_animation
//=============================================================================
void get_animation (t_enum_animation anim_id, t_struct_animation* p_animation)
{
    p_animation->id          = vec_call_back_anim[anim_id].id;
    p_animation->high_speed  = vec_call_back_anim[anim_id].high_speed;
    p_animation->started     = vec_call_back_anim[anim_id].started;
    p_animation->description = vec_call_back_anim[anim_id].description;
    p_animation->callback    = vec_call_back_anim[anim_id].callback;
}// get_animation

//=============================================================================
//============================================ get_is_auto_anim_rotation_enable
//=============================================================================
BOOL get_is_auto_anim_rotation_enable()
{
    return is_auto_anim_rotation_enable;
}// get_is_auto_anim_rotation_enable

//=============================================================================
//============================================ set_is_auto_anim_rotation_enable
//=============================================================================
void set_is_auto_anim_rotation_enable(BOOL the_is_auto_anim_rotation_enable)
{
    is_auto_anim_rotation_enable = the_is_auto_anim_rotation_enable;
}// set_is_auto_anim_rotation_enable

//=============================================================================
//============================================= get_is_anim_on_button_up_enable
//=============================================================================
BOOL get_is_anim_on_button_up_enable()
{
    return is_anim_on_button_up_enable;
}// get_is_anim_on_button_up_enable

//=============================================================================
//============================================= set_is_anim_on_button_up_enable
//=============================================================================
void set_is_anim_on_button_up_enable(BOOL the_is_anim_on_button_up_enable)
{
    if (current_callback_anim < ANIM_UNKNOWN)
    {
        send_animation_telemetry(current_callback_anim, the_is_anim_on_button_up_enable ? "starting" : "stopping");
        vec_call_back_anim[current_callback_anim].started = the_is_anim_on_button_up_enable;
    }
    
    is_anim_on_button_up_enable = the_is_anim_on_button_up_enable;
}// set_is_anim_on_button_up_enable

//=============================================================================
//=================================================== get_current_callback_anim
//=============================================================================
t_enum_animation get_current_callback_anim()
{
    return current_callback_anim;
}// get_current_callback_anim

//=============================================================================
//==================================================== is_smooth_button_anim_on
//=============================================================================
BOOL is_smooth_button_anim_on()
{
    return ((current_callback_anim == ANIM_BUTTON_SMOOTH_SLOW) || (current_callback_anim == ANIM_BUTTON_SMOOTH_FAST)) ? TRUE : FALSE;
}// get_current_callback_anim

//=============================================================================
//=================================================== set_current_callback_anim
//=============================================================================
void set_current_callback_anim(t_enum_animation the_current_callback_anim)
{
    if (the_current_callback_anim >= ANIM_UNKNOWN)
    {
        send_alarm_telemetry(ALARM_ERROR, "firmware", "invalid animation id");
        return;
    }
    
    if (vec_call_back_anim[the_current_callback_anim].started)
    {
        send_animation_telemetry(the_current_callback_anim, "stopping");
        vec_call_back_anim[the_current_callback_anim].started = FALSE;
    }
    
    current_callback_anim = the_current_callback_anim;
    send_animation_telemetry(current_callback_anim, "selected");
}// set_current_callback_anim

//=============================================================================
//======================================================= run_current_animation
//=============================================================================
void run_current_animation(BOOL high_speed)
{
    if ((current_callback_anim >= ANIM_UNKNOWN) || (vec_call_back_anim[current_callback_anim].high_speed != high_speed))
    {
        return;
    }
    
    if (vec_call_back_anim[current_callback_anim].started)
    {
        send_animation_telemetry(current_callback_anim, "started");
        vec_call_back_anim[current_callback_anim].started = TRUE;
    }
    
    vec_call_back_anim[current_callback_anim].callback();
}// run_current_animation

/* [] END OF FILE */



