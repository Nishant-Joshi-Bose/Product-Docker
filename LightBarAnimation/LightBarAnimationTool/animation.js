//------------------------------------------------------------------------------
//-------------------------------------------------------------------- constants
//------------------------------------------------------------------------------
const NUMBER_OF_WHITE   = 13;
const MIDDLE_WHITE      = Math.floor(NUMBER_OF_WHITE / 2);
const MAX_LED_INTENSITY = 4095;
const DIM_LED_INTENSITY = Math.floor(MAX_LED_INTENSITY/16);
const LED_INTENSITY_1 = Math.floor(DIM_LED_INTENSITY*2);
const LED_INTENSITY_2 = Math.floor(DIM_LED_INTENSITY*3);
const LED_INTENSITY_3 = Math.floor(DIM_LED_INTENSITY*4);
const MID_LED_INTENSITY = Math.floor(MAX_LED_INTENSITY/2);
const BRIGHT_LED_INTENSITY = Math.floor(MAX_LED_INTENSITY *3/4);

//------------------------------------------------------------------------------
//-------------------------------------------------------------------- variables
//------------------------------------------------------------------------------
var web_socket         = null;
var white_leds         = new Uint16Array(NUMBER_OF_WHITE);
var left_colored_led   = {red: 0, green: 0, blue: 0};
var right_colored_led  = {red: 0, green: 0, blue: 0};
var color_vec          = new Array("RED" , "GREEN", "YELLOW"      , "CYAN"        , "MAGENTA");
var white_pattern_vec  = new Array("FULL", "W"    , "MIDDLE-LARGE", "MIDDLE-SMALL", "NONE"   );
var alexa_listening_pattern_vec  = new Array("ENDS", "MIDDLE1", "MIDDLE2", "MIDDLE3","MIDDLE4" , "MIDDLE-LARGE", "MIDDLE-LARGE", "MIDDLE-LARGE", "MIDDLE-LARGE", "MIDDLE-LARGE","NONE");
var alexa_active_listening_pattern_vec  = new Array("MIDDLE-LARGE", "MIDDLE-SMALL", "MIDDLE-XLARGE", "MIDDLE-SMALL","MIDDLE-XLARGE", "MIDDLE-XLARGE","MIDDLE-XLARGE","NONE" );
var alexa_thinking_pattern_vec  = new Array("MIDDLE-LARGE", "MIDDLE-XLARGE", "MIDDLE-XXLARGE", "MIDDLE-XXXLARGE","MIDDLE-XXXXLARGE", "ALL","NONE" );
var alexa_speaking_pattern_vec  = new Array("ALL-DIM","ALL-INTENSITY-1","ALL-INTENSITY-2","ALL-INTENSITY-3", "ALL-MID", "ALL","ALL","ALL-MID","ALL-INTENSITY-3","ALL-INTENSITY-2","ALL-INTENSITY-1","ALL-DIM" );
var left_colored_led   = {red: 0, green: 0, blue: 0};
var right_colored_led  = {red: 0, green: 0, blue: 0};
var color_index        = 0;
var white_index        = 0;
var alexa_active_listening_index        = 0;
var alexa_thinking_index        = 0;
var alexa_speaking_index        = 0;
var frame_number       = 0;
var animation_interval = 0;
var frame_rate         = 1;

//------------------------------------------------------------------------------
//---------------------------------------------------------------- set_led_strip
//------------------------------------------------------------------------------
//                                                          LR-LG-LB-W0-W1-W2-W3-W4-W5-W6-W7-W8-W9-10-11-12-RR-RG-RB
//{"event":"cmd","type":"led","action":"set strip","strip":"22-88-33-00-11-22-33-44-55-66-77-88-99-00-11-22-99-00-99"}
//------------------------------------------------------------------
//function set_led_strip(ws, left_colored_led, white_leds, right_colored_led) {
function set_led_strip(ws, leds) {
    var cmd_header        = '{"event":"cmd","type":"led","action":"set strip","strip":"';
    var cmd_trailer       = '"}';
    //var cmd_left_colored  = left_colored_led .red + '-' + left_colored_led .green + '-' + left_colored_led .blue;
    //var cmd_right_colored = right_colored_led.red + '-' + right_colored_led.green + '-' + right_colored_led.blue;
    //var cmd_white         = "";
    var cmd_leds	    = "";

    //for (i = 0; i <  NUMBER_OF_WHITE; i++) {
        //cmd_white += white_leds[i] + '-';
    //}// for all the whites
    //PJ - <TBD> may need to rework on this logic
    for (i = 0; i <  leds.length; i++) {
        cmd_leds += leds[i] + '-';
    }// for all the whites

    //var cmd = cmd_header + cmd_left_colored + '-' + cmd_white + cmd_right_colored + cmd_trailer;
    var cmd = cmd_header + cmd_leds + cmd_trailer;

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
function clear_led_strip(ws, leds, send_cmd) {

    for (i = 0; i < leds.length; i++) {
        leds[i] = 0;
    }


    if (send_cmd) {
        set_led_strip(ws, leds);
    }
}// clear_led_strip


//------------------------------------------------------------------------------
//------------------------------------------------------------ set_white_pattern
//------------------------------------------------------------------------------
function set_alexa_listening_pattern(pattern, white_leds) {

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
//------------------------------------------------------------ set_white_pattern
//------------------------------------------------------------------------------
function set_white_pattern(pattern, white_leds) {

    for (i = 0; i < NUMBER_OF_WHITE; i++) {
		white_leds[i] = 0;
    }

	var cmd_white='';

    switch (pattern) {
        case 'ENDS'        : white_leds[0                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[1                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 2] = MAX_LED_INTENSITY;
							for (i = 0; i <  NUMBER_OF_WHITE; i++) {
								cmd_white += white_leds[i] + '-';
							}// for all the whites
							console.log(cmd_white);
                             break;

        case 'MIDDLE1'     : white_leds[1                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[2                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 3] = MAX_LED_INTENSITY;
							for (i = 0; i <  NUMBER_OF_WHITE; i++) {
								cmd_white += white_leds[i] + '-';
							}// for all the whites
							console.log(cmd_white);
                             break;

        case 'MIDDLE2'     : white_leds[2                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 3] = MAX_LED_INTENSITY;
                             white_leds[3                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 4] = MAX_LED_INTENSITY;
							for (i = 0; i <  NUMBER_OF_WHITE; i++) {
								cmd_white += white_leds[i] + '-';
							}// for all the whites
							console.log(cmd_white);
                             break;

        case 'MIDDLE3'     : white_leds[3                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 4] = MAX_LED_INTENSITY;
                             white_leds[4                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 5] = MAX_LED_INTENSITY;
							for (i = 0; i <  NUMBER_OF_WHITE; i++) {
								cmd_white += white_leds[i] + '-';
							}// for all the whites
							console.log(cmd_white);
                             break;

        case 'MIDDLE4'     : white_leds[4                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 5] = MAX_LED_INTENSITY;
                             white_leds[5                  ] = MAX_LED_INTENSITY;
                             white_leds[NUMBER_OF_WHITE - 6] = MAX_LED_INTENSITY;
							for (i = 0; i <  NUMBER_OF_WHITE; i++) {
								cmd_white += white_leds[i] + '-';
							}// for all the whites
							console.log(cmd_white);
                             break;

        case 'MIDDLE-LARGE': 
			   white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
			   white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                           white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
							for (i = 0; i <  NUMBER_OF_WHITE; i++) {
								cmd_white += white_leds[i] + '-';
							}// for all the whites
							console.log(cmd_white);
			   break;
        case 'NONE'       : 
			   break;
        default            : console.error("error: unsupported led pattern: " + pattern);
                             break;
    }// switch pattern
}// set_white_pattern


//------------------------------------------------------------------------------
//_------------------------------------------ set_alexa_active_listening_pattern
//------------------------------------------------------------------------------
function set_alexa_active_listening_pattern(pattern, white_leds) {

    for (i = 0; i < NUMBER_OF_WHITE; i++) {
        white_leds[i] = 0;
    }

    switch (pattern) {
        case 'MIDDLE-LARGE':
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             break;
        case 'MIDDLE-SMALL': 
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             break;
        case 'MIDDLE-XLARGE':
                             white_leds[MIDDLE_WHITE - 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 3] = MAX_LED_INTENSITY;
                             break;

        case 'NONE'        : break;
        default            : console.error("error: unsupported led pattern: " + pattern);
                             break;
    }// switch pattern
}// set_alexa_active_pattern


//------------------------------------------------------------------------------
//_------------------------------------------ set_alexa_thinking_pattern
//------------------------------------------------------------------------------
function set_alexa_thinking_pattern(pattern, white_leds) {

    for (i = 0; i < NUMBER_OF_WHITE; i++) {
        white_leds[i] = 0;
    }

    switch (pattern) {
        case 'MIDDLE-LARGE':
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             break;
        case 'MIDDLE-XLARGE':
                             white_leds[MIDDLE_WHITE - 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 3] = MAX_LED_INTENSITY;
                             break;

        case 'MIDDLE-XXLARGE':
                             white_leds[MIDDLE_WHITE - 4] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 4] = MAX_LED_INTENSITY;
                             break;

        case 'MIDDLE-XXXLARGE':
                             white_leds[MIDDLE_WHITE - 5] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 4] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 4] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 5] = MAX_LED_INTENSITY;
                             break;

        case 'MIDDLE-XXXXLARGE':
                             white_leds[MIDDLE_WHITE - 6] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 5] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 4] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE - 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE    ] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 1] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 2] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 3] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 4] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 5] = MAX_LED_INTENSITY;
                             white_leds[MIDDLE_WHITE + 6] = MAX_LED_INTENSITY;
                             break;

        case 'ALL'        : 
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = MAX_LED_INTENSITY;
							}
							break;
        case 'NONE'        : break;
        default            : console.error("error: unsupported led pattern: " + pattern);
                             break;
    }// switch pattern
}// set_alexa_active_pattern


//------------------------------------------------------------------------------
//_------------------------------------------ set_alexa_speaking_pattern
//------------------------------------------------------------------------------
function set_alexa_speaking_pattern(pattern, white_leds) {

    for (i = 0; i < NUMBER_OF_WHITE; i++) {
        white_leds[i] = 0;
    }

    switch (pattern) {
        case 'ALL-DIM':
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = DIM_LED_INTENSITY;
							}
                             break;
        case 'ALL-INTENSITY-1':
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = LED_INTENSITY_1;
							}
                             break;
        case 'ALL-INTENSITY-2':
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = LED_INTENSITY_2;
							}
                             break;
        case 'ALL-INTENSITY-3':
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = LED_INTENSITY_3;
							}
                             break;
        case 'ALL-MID': 
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = MID_LED_INTENSITY;
							}
                             break;
        case 'ALL-BRIGHT': 
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = BRIGHT_LED_INTENSITY;
							}
                             break;
        case 'ALL': 
							for (i = 0; i < NUMBER_OF_WHITE; i++) {
								white_leds[i] = MAX_LED_INTENSITY;
							}
                             break;

        case 'NONE'        : break;
        default            : console.error("error: unsupported led pattern: " + pattern);
                             break;
    }// switch pattern
}// set_alexa_active_pattern

//------------------------------------------------------------------------------
//---------------------------------------------------------------- run_animation
//------------------------------------------------------------------------------
function run_animation () {

    clear_led_strip(web_socket, left_colored_led, white_leds, right_colored_led, false);

    set_white_pattern(alexa_listening_pattern_vec[white_index++], white_leds);
    white_index %= alexa_listening_pattern_vec.length;
    console.log(white_index);
    set_led_strip(web_socket, left_colored_led, white_leds, right_colored_led);
    frame_number++;
}// run_animation


//------------------------------------------------------------------------------
//------------------------------------------------------------- active listening 
//------------------------------------------------------------------------------
function run_animation_alexa_active_listening () {

    set_alexa_active_listening_pattern(alexa_active_listening_pattern_vec[alexa_active_listening_index++], white_leds);
    alexa_active_listening_index %= alexa_active_listening_pattern_vec.length;

    set_led_strip(web_socket, left_colored_led, white_leds, right_colored_led);
    frame_number++;
}// run_animation


//------------------------------------------------------------------------------
//------------------------------------------------------------- thinkging 
//------------------------------------------------------------------------------
function run_animation_alexa_thinking() {

    set_alexa_thinking_pattern(alexa_thinking_pattern_vec[alexa_thinking_index++], white_leds);
    alexa_thinking_index %= alexa_thinking_pattern_vec.length;

    set_led_strip(web_socket, left_colored_led, white_leds, right_colored_led);
    frame_number++;
}// run_animation


//------------------------------------------------------------------------------
//------------------------------------------------------------- speaking 
//------------------------------------------------------------------------------
function run_animation_alexa_speaking() {

    set_alexa_speaking_pattern(alexa_speaking_pattern_vec[alexa_speaking_index++], white_leds);
    alexa_speaking_index %= alexa_speaking_pattern_vec.length;

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
            //get javascript animations
            var animation      = data.animation;
	    if (animation == 'Alexa Listening')
		animation_interval = setInterval(run_animation, 1000 / frame_rate);
            else if (animation == 'Alexa Active Listening')
		animation_interval = setInterval(run_animation_alexa_active_listening, 1000 / frame_rate);
            else if (animation == 'Alexa Thinking')
		animation_interval = setInterval(run_animation_alexa_thinking, 1000 / frame_rate);
            else if (animation == 'Alexa Speaking')
	animation_interval = setInterval(run_animation_alexa_speaking, 1000 / frame_rate);
            else //default 
		animation_interval = setInterval(run_animation, 1000 / frame_rate);
	//	
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
            clear_led_strip(web_socket, leds, true);
            self.postMessage('WORKER LED STRIP CLEARED');
            break;
        }// clear led strip

        case 'set led strip' : {
            set_led_strip(web_socket, data.leds, true);
            self.postMessage('WORKER LED STRIP SET');
            break;
        }// clear led strip

        default: {
            self.postMessage('WORKER UNSUPPORTED COMMAND: ' + data.cmd);
        }// default
    };// switch on the command
}, false);
