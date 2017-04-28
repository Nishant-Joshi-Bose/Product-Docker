//------------------------------------------------------------------------------
//-------------------------------------------------------------------- constants
//------------------------------------------------------------------------------
const NUMBER_OF_WHITE   = 13;
const MIDDLE_WHITE      = NUMBER_OF_WHITE / 2;
const MAX_LED_INTENSITY = 4095;

//------------------------------------------------------------------------------
//-------------------------------------------------------------------- variables
//------------------------------------------------------------------------------
var web_socket         = null;
var white_leds         = new Uint16Array(NUMBER_OF_WHITE);
var left_colored_led   = {red: 0, green: 0, blue: 0};
var right_colored_led  = {red: 0, green: 0, blue: 0};
var color_vec          = new Array("RED" , "GREEN", "YELLOW"      , "CYAN"        , "MAGENTA");
var white_pattern_vec  = new Array("FULL", "W"    , "MIDDLE-LARGE", "MIDDLE-SMALL", "NONE"   );
var left_colored_led   = {red: 0, green: 0, blue: 0};
var right_colored_led  = {red: 0, green: 0, blue: 0};
var color_index        = 0;
var white_index        = 0;
var frame_number       = 0;
var animation_interval = 0;
var frame_rate         = 1;

//------------------------------------------------------------------------------
//---------------------------------------------------------------- set_led_strip
//------------------------------------------------------------------------------
//                                                          LR-LG-LB-W0-W1-W2-W3-W4-W5-W6-W7-W8-W9-10-11-12-RR-RG-RB
//{"event":"cmd","type":"led","action":"set strip","strip":"22-88-33-00-11-22-33-44-55-66-77-88-99-00-11-22-99-00-99"}
//------------------------------------------------------------------
function set_led_strip(ws, left_colored_led, white_leds, right_colored_led) {
    var cmd_header        = '{"event":"cmd","type":"led","action":"set strip","strip":"';
    var cmd_trailer       = '"}';
    var cmd_left_colored  = left_colored_led .red + '-' + left_colored_led .green + '-' + left_colored_led .blue;
    var cmd_right_colored = right_colored_led.red + '-' + right_colored_led.green + '-' + right_colored_led.blue;
    var cmd_white         = "";

    for (i = 0; i <  NUMBER_OF_WHITE; i++) {
        cmd_white += white_leds[i] + '-';
    }// for all the whites

    var cmd = cmd_header + cmd_left_colored + '-' + cmd_white + cmd_right_colored + cmd_trailer;

    ws.send(cmd);
}// set_led_strip

//------------------------------------------------------------------------------
//--------------------------------------------------------- set_left_right_color
//------------------------------------------------------------------------------
function set_left_right_color(left_colored_led, color, right_colored_led) {

    switch (color)
    {
        case 'BLACK'  : left_colored_led.red   = right_colored_led.red   = 0;
                        left_colored_led.green = right_colored_led.green = 0;
                        left_colored_led.blue  = right_colored_led.blue  = 0;
                        break;
        case 'WHITE'  : left_colored_led.red   = right_colored_led.red   = MAX_LED_INTENSITY;
                        left_colored_led.green = right_colored_led.green = MAX_LED_INTENSITY;
                        left_colored_led.blue  = right_colored_led.blue  = MAX_LED_INTENSITY;
                        break;
        case 'RED'    : left_colored_led.red   = right_colored_led.red   = MAX_LED_INTENSITY;
                        left_colored_led.green = right_colored_led.green = 0;
                        left_colored_led.blue  = right_colored_led.blue  = 0;
                        break;
        case 'GREEN'  : left_colored_led.red   = right_colored_led.red   = 0;
                        left_colored_led.green = right_colored_led.green = MAX_LED_INTENSITY;
                        left_colored_led.blue  = right_colored_led.blue  = 0;
                        break;
        case 'YELLOW' : left_colored_led.red   = right_colored_led.red   = MAX_LED_INTENSITY;
                        left_colored_led.green = right_colored_led.green = MAX_LED_INTENSITY;
                        left_colored_led.blue  = right_colored_led.blue  = 0;
                        break;
        case 'CYAN'   : left_colored_led.red   = right_colored_led.red   = 0;
                        left_colored_led.green = right_colored_led.green = MAX_LED_INTENSITY;
                        left_colored_led.blue  = right_colored_led.blue  = MAX_LED_INTENSITY;
                        break;
        case 'MAGENTA': left_colored_led.red   = right_colored_led.red   = MAX_LED_INTENSITY;
                        left_colored_led.green = right_colored_led.green = 0;
                        left_colored_led.blue  = right_colored_led.blue  = MAX_LED_INTENSITY;
                        break;
        default       : console.error("unsupported color: " + color);
    }// switch color
}// set_left_right_color

//------------------------------------------------------------------------------
//-------------------------------------------------------------- clear_led_strip
//------------------------------------------------------------------------------
function clear_led_strip(ws, left_colored_led, white_leds, right_colored_led, send_cmd) {
    left_colored_led.red   = 0;
    left_colored_led.green = 0;
    left_colored_led.blue  = 0;

    for (i = 0; i < NUMBER_OF_WHITE; i++) {
        white_leds[i] = 0;
    }

    right_colored_led.red   = 0;
    right_colored_led.green = 0;
    right_colored_led.blue  = 0;

    if (send_cmd) {
        set_led_strip(ws, left_colored_led, white_leds, right_colored_led);
    }
}// clear_led_strip

//------------------------------------------------------------------------------
//------------------------------------------------------------ set_white_pattern
//------------------------------------------------------------------------------
function set_white_pattern(pattern, white_leds) {

    for (i = 0; i < NUMBER_OF_WHITE; i++) {
        white_leds[i] = 0;
    }

    switch (pattern) {
        case 'FULL'        : for (i = 0; i < NUMBER_OF_WHITE; i++) {
                                white_leds[i] = MAX_LED_INTENSITY;
                             }
                             break;
        case 'W'           : white_leds[0                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE       ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1   ] = MAX_LED_INTENSITY;
                             break;
        case 'MIDDLE-LARGE': for (i = MIDDLE_WHITE - 1; i < MIDDLE_WHITE + 2; i++) {
                                 white_leds[i] = MAX_LED_INTENSITY;
                             }
                             break;

        case 'MIDDLE-SMALL': white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             break;
        case 'NONE'        : break;
        default            : console.error("error: unsupported led pattern: " + pattern);
                             break;
    }// switch pattern
}// set_white_pattern

//------------------------------------------------------------------------------
//---------------------------------------------------------------- run_animation
//------------------------------------------------------------------------------
function run_animation () {
    clear_led_strip(web_socket, left_colored_led, white_leds, right_colored_led, false);

    if (frame_number & 0x01) {
        set_left_right_color(left_colored_led, 'BLACK', right_colored_led);
    }
    else {
        set_left_right_color(left_colored_led, color_vec[color_index++], right_colored_led);
        color_index %= color_vec.length;
    }

    set_white_pattern(white_pattern_vec[white_index++], white_leds);
    white_index %= white_pattern_vec.length;

    set_led_strip(web_socket, left_colored_led, white_leds, right_colored_led);
    frame_number++;
}// run_animation

//------------------------------------------------------------------------------
//--------------------------------------------------------------- event listener
//------------------------------------------------------------------------------
self.addEventListener('message', function(e) {
    var data = e.data;

    switch (data.cmd) {
        case 'init': {
            web_socket         = new WebSocket (data.msg);
            web_socket.onopen  = function(ev) { console.log(ev); };
            web_socket.onerror = function(ev) { console.log(ev); };
            web_socket.onclose = function(ev) { console.log(ev); };

            self.postMessage('WORKER INITIALIZED');
            break;
        }// init

        case 'start': {
            frame_rate         = parseInt(data.msg);
            animation_interval = setInterval(run_animation, 1000 / frame_rate);
            self.postMessage('WORKER STARTED');
            break;
        }// start

        case 'stop': {
            if (animation_interval != 0) {
                clearInterval(animation_interval);
                animation_interval = 0;
            }

            self.postMessage('WORKER STOPPED');
            break;
        }// stop

        case 'uninit' : {
            if (animation_interval != 0) {
                clearInterval(animation_interval);
            }

            if (web_socket != null) {
                web_socket.close();
                web_socket = null;
            }

            self.postMessage('WORKER UNINITIALIZED');
            self.close(); // Terminates the worker.
            break;
        }// uninit

        case 'set frame rate': {
            frame_rate = parseInt(data.msg);

            if (animation_interval != 0) {
                clearInterval(animation_interval);
                animation_interval = setInterval(run_animation, 1000 / frame_rate);
            }

            self.postMessage('WORKER FRAME RATE SET');
            break;
        }// set frame rate

        case 'clear led strip' : {
            clear_led_strip(web_socket, left_colored_led, white_leds, right_colored_led, true);
            self.postMessage('WORKER LED STRIP CLEARED');
            break;
        }// clear led strip

        case 'set led strip' : {
            set_led_strip(web_socket, data.left, data.whites, data.right, true);
            self.postMessage('WORKER LED STRIP SET');
            break;
        }// clear led strip

        default: {
            self.postMessage('WORKER UNSUPPORTED COMMAND: ' + data.cmd);
        }// default
    };// switch on the command
}, false);
