/*
 * capsensehdlr.c
 *
 *  Created on: Jun 13, 2017
 *      Author: mm1005301
 */

#include <project.h>
#include "util.h"

#define MAX_CAPSENSE_BUTTONS 24
#define MAX_CAPSENSE_SLIDERS 2

typedef struct {
    uint8_t id;
    uint32_t pressed;
} Button_t;

typedef struct {
    uint8_t id;
    uint32_t lastPos;
} Slider_t;

static Button_t buttons[MAX_CAPSENSE_BUTTONS];
static Slider_t sliders[MAX_CAPSENSE_SLIDERS];
static uint8_t nButtons = 0;
static uint8_t nSliders = 0;

void CapsenseHandlerInit(void)
{
    CapSense_Start();
    memset(buttons, 0, sizeof(buttons));
    for (uint8_t i = 0; i < MAX_CAPSENSE_SLIDERS; i++)
    {
        sliders[i].id = 0;
        sliders[i].lastPos = CapSense_SLIDER_NO_TOUCH;
    }
}

static void SendButtonEvent(const Button_t *button)
{

}

void CapsenseHandlerScanButtons(void)
{
    for (uint8_t i = 0; i < nButtons; i++)
    {
        uint32_t buttonActive = CapSense_IsWidgetActive(buttons[i].id);
        if (buttonActive != buttons[i].pressed)
        {
            buttons[i].pressed = buttonActive;
            SendButtonEvent((const)&buttons[i]);
        }
    }
}

void CapsenseHandlerScanSliders(void)
{
    for (uint8_t i = 0; i < nSliders; i++)
    {
        cap_cur_pos            = CapSense_GetCentroidPos(0);
        cap_sense_is_untouched = ((cap_cur_pos == CapSense_SLIDER_NO_TOUCH) && (cap_old_pos == CapSense_SLIDER_NO_TOUCH));

        if (cap_sense_is_untouched)
        {
            if (! cap_sense_was_untouched)
            {
                if (get_is_tracker_enable())
                {
                    set_all_leds_intensity(0x00);
                }

                finger_direction = DIRECT_STOP;
                send_button_telemetry(get_button_pos(cap_old_pos, cap_cur_pos), "up", finger_direction, cap_cur_pos, cap_old_pos);
                cap_cur_pos = CapSense_SLIDER_NO_TOUCH;
                cap_old_pos = CapSense_SLIDER_NO_TOUCH;
                led_pos     = NUMBER_OF_WHITE_LEDS + 1;

                if (get_is_auto_anim_rotation_enable())
                {
                    current_callback_anim = get_current_callback_anim();
                    current_callback_anim++;
                    current_callback_anim %= ANIM_UNKNOWN;
                    set_current_callback_anim(current_callback_anim);
                }
            }

            if (get_is_anim_on_button_up_enable())
            {
                run_current_animation(FALSE);
            }
        }// if capsense is untouched
        else
        {
            if (cap_sense_was_untouched)
            {
                tmp = get_button_pos(cap_old_pos, cap_cur_pos);
                reset_timer();
                send_button_telemetry(tmp, "down"    , finger_direction, cap_cur_pos, cap_old_pos);
                send_button_telemetry(tmp, "position", finger_direction, cap_cur_pos, cap_old_pos);
                if (get_is_anim_on_button_up_enable())
                {
                    set_all_leds_intensity(0x00);
                    send_animation_telemetry(current_callback_anim, "stopping");
                }
            }

            if ((cap_cur_pos != CapSense_SLIDER_NO_TOUCH) && (cap_cur_pos != cap_old_pos))
            {
                if (cap_old_pos != CapSense_SLIDER_NO_TOUCH)
                {
                    finger_direction = (cap_cur_pos > cap_old_pos ? DIRECT_RIGHT : DIRECT_LEFT);
                }

                new_led_pos = display_finger_tracker (cap_old_pos, cap_cur_pos);
                if (new_led_pos != led_pos)
                {
                    led_pos = new_led_pos;
                    send_button_telemetry(get_button_pos(cap_old_pos, cap_cur_pos), "position", finger_direction, cap_cur_pos, cap_old_pos);
                }
            }

            cap_old_pos = cap_cur_pos;
        }// else, capsense is touched
        cap_sense_was_untouched = cap_sense_is_untouched;
    }
}
