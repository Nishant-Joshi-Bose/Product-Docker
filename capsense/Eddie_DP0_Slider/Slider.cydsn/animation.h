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
#ifndef ANIMATION_H
#define ANIMATION_H

#include <project.h>
#include "util.h"
#include "led.h"
    
//=============================================================================
//======================================================== types and structures
//=============================================================================    
typedef enum enum_animation
{
    ANIM_BUTTON_SMOOTH_SLOW = 0,
    ANIM_BUTTON_SMOOTH_FAST    ,
    ANIM_LEFT_RIGHT            ,
    ANIM_INTENSITY             ,
    ANIM_2_LEDS                ,
    ANIM_RED_INTENSITY         ,
    ANIM_GREEN_INTENSITY       ,
    ANIM_BLUE_INTENSITY        ,  
    ANIM_RED_WHITE_INTENSITY   ,
    ANIM_GREEN_WHITE_INTENSITY ,
    ANIM_BLUE_WHITE_INTENSITY  ,
    ANIM_COLORS_INTENSITY      ,
    ANIM_RAINBOW_COLORS        ,
    ANIM_FIX_WHITE             ,
    ANIM_UNKNOWN               // keep it last, not an animation
}t_enum_animation;

typedef void (*t_callback_anim) (void);
typedef struct struct_animation
{
    t_enum_animation id         ;
    BOOL             high_speed ;
    BOOL             started    ;
    t_callback_anim  callback   ;
    const char*      description; 
}t_struct_animation;

//=============================================================================
//========================================================= function prototypes
//============================================================================= 
void             animate_left_right              (void);
void             animate_intensity               (void);
void             animate_2_leds                  (void);
void             animate_random                  (void);
void             animate_red_intensity           (void);
void             animate_green_intensity         (void);
void             animate_blue_intensity          (void);
void             animate_red_white_intensity     (void);
void             animate_green_white_intensity   (void);
void             animate_blue_white_intensity    (void);
void             animate_all_colors              (void);
void             animate_rainbow_colors          (void);
void             animate_fix_white               (void);
void             animate_color_intensity         (t_enum_primary_color color);
void             animate_button_smooth           (uint16 breather_skip_cycle);
void             animate_button_smooth_slow      (void);
void             animate_button_smooth_fast      (void);
void             get_animation                   (t_enum_animation anim_id, t_struct_animation* p_animation);
t_enum_animation get_current_callback_anim       (void);
void             set_current_callback_anim       (t_enum_animation current_callback_anim);
BOOL             get_is_auto_anim_rotation_enable(void);
void             set_is_auto_anim_rotation_enable(BOOL is_auto_anim_rotation_enable);
BOOL             get_is_anim_on_button_up_enable (void);
void             set_is_anim_on_button_up_enable (BOOL is_anim_on_button_up_enable);
BOOL             is_smooth_button_anim_on        (void);
void             run_current_animation           (BOOL high_speed);

#endif //ANIMATION_H
/* [] END OF FILE */
