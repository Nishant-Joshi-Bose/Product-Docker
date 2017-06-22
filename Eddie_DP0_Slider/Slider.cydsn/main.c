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
#include <project.h>
#include "util.h"
#include "animation.h"
#include "led.h"
#include "comms.h"
#include "capsensehdlr.h"

/*
 * The PSoC controls up to 24 buttons, 24 leds, and 2 sliders.
 *
 * The pin connections to the capsense buttons and sliders are configured
 * using the PSoC creator configuration tool.  During build, the tool will
 * generate code for the attached hardware.  Our hand-written code will
 * automatically determine how many buttons and sliders (if any) are
 * connected based on the generated code from this configuration.
 * There currently are always 24 LEDs.  If there are less, i.e. nothing
 * is connected to some of the LED driver pins, it doesn't matter.  Whether
 * the LEDs are RGB or white doesn't matter.  RGB is just treated as 3
 * separate LEDs.  It's up to the product controller (LPM or QC SOC) to
 * know what's actually hooked up and be able to set the proper LED
 * patterns, and what to expect for slider/button data.
 *
 * This makes the PSoC Lightbar/Capense module a reusable component
 * code-wise.  You can even use different PSoC chips with more/less pins.
 *
 * The PSoC does not use an RTOS; instead it just has a main loop that
 * processes everything.
 *
@startuml
title Main Loop
participant PSoC as p
participant Slider as s
participant Button as b
participant LPM as l
loop Main
p -> l: read/process
p -> s: read/process
p -> b: read/process
end
@enduml
 * The PSoC communicates with the LPM via I2C as a slave (LPM is master).
 * Since it has to send capsense events asynchronously back to the LPM,
 * it sets a gpio to notify the LPM when there is something to read.
 * The LPM can use this as an interrupt to do the actual read.
 * The gpio stays set until the read is done.
 *
@startuml
title I2C Async Communications
participant PSoC as p
participant LPM as l
p -> l: set gpio
l -> l: handle interrupt
l -> p: i2c read
p -> l: clear gpio
@enduml
 *
 */

// TODO handle the tactile button for Eddie
// Try to genericize it like the capsense stuff
// Debounce:
// See http://www.cypress.com/documentation/application-notes/an60024-psoc-3-psoc-4-psoc-5lp-switch-debouncer-and-glitch-filter
// http://www.cypress.com/documentation/component-datasheets/debouncer
// See Ted's PIN_LED example for gpio banks (we may want to use that for all the tact buttons if there's more than one)

int main()
{
    CyGlobalIntEnable; // enable global interrupts before starting capsense and I2C blocks

    LedsInit();
    AnimationInit();
    CommsInit();

    CapsenseHandlerInit();

    for(;;)
    {
        CommsHandleIncoming();
        CapsenseHandlerScan();
        AnimationRun();
    }
}
