#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <strings.h>

#include "mongoose.h"
#include "LightBarAnimation.h"

#ifndef BOOL
#define BOOL unsigned char
#endif // BOOL

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif // TRUE

#define UNUSED(x) (void)(x)

#define MAX_LED_INTENSITY 4095
#define DEMO_WITH_SOUND   0    // Set to 1 to enable the demo with sound

typedef enum enum_led_strip_index
{
    E_LED_LR = 0,
    E_LED_LG    ,
    E_LED_LB    ,
    E_LED_W1    ,
    E_LED_W2    ,
    E_LED_W3    ,
    E_LED_W4    ,
    E_LED_W5    ,
    E_LED_W6    ,
    E_LED_W7    ,
    E_LED_W8    ,
    E_LED_W9    ,
    E_LED_W10   ,
    E_LED_W11   ,
    E_LED_W12   ,
    E_LED_W13   ,
    E_LED_RR    ,
    E_LED_RG    ,
    E_LED_RB    ,
    E_LED_MAX
}t_enum_led_strip_index;

typedef unsigned short int t_led_strip_vec[E_LED_MAX];

static const char*               s_default_serial_port_name   = "/dev/ttyUSB1";
static const char*               s_psoc_cmd_telemetry_enable  = "{\"event\":\"cmd\",\"type\":\"telemetry\",\"action\":\"enable\"}";
static const char*               s_psoc_cmd_telemetry_disable = "{\"event\":\"cmd\",\"type\":\"telemetry\",\"action\":\"disable\"}";
static const char*               s_psoc_cmd_animation_enable  = "{\"event\":\"cmd\",\"type\":\"animation\",\"action\":\"resume\"}";
static const char*               s_psoc_cmd_animation_disable = "{\"event\":\"cmd\",\"type\":\"animation\",\"action\":\"stop\"}";
static const char*               s_psoc_cmd_set_strip_head    = "{\"event\":\"cmd\",\"type\":\"led\",\"action\":\"set strip\",\"strip\":\"";
static const char*               s_psoc_cmd_set_strip_tail    = "\"}";
static const char*               s_start_anim                 = "start anim";
static const char*               s_stop_anim                  = "stop anim";
static const char*               s_set_rate_anim              = "set rate anim";
static const char*               s_enable_show_set_strip      = "enable show set strip";
static const char*               s_disable_show_set_strip     = "disable show set strip";
static const char*               s_sound_file_slide           = "slide.raw";
static const char*               s_sound_file_peek            = "peek.raw";
static const char*               s_sound_file_select_preset   = "select-preset.raw";
static const char*               s_sound_file_directory       = "/home/pi/Eddie-Demo/capsense/sounds";
static const char*               s_save_pattern               = "save pattern";
static const char*               s_get_animation              = "get animation";
static const char*               s_play_animation             = "play animation";
static const char*               s_delete_animation           = "delete animation";
static const char*               s_save_animation             = "save";
static const char*               s_get_configuration          = "get configuration";


static sig_atomic_t              s_signal_received             = 0;
static const char*               s_http_port                   = "8001";
static struct mg_connection*     s_network_connection          = NULL;
static int                       s_serial_port_fd              = -1;
static BOOL                      s_stop_thread                 = FALSE;
static char                      s_end_of_cmd                  = '}';
static BOOL                      s_stop_animation_side_by_side = FALSE;
static unsigned int              s_anim_frame_rate_per_sec     = 1;
static pthread_t                 s_thread_animation_id         = INT_MAX;
static BOOL                      s_show_set_strip              = TRUE;
static struct mg_serve_http_opts s_http_server_opts;

extern void  cfmakeraw                    (struct termios *__termios_p); // this is hidden from termios.h
static BOOL  is_websocket                 (const struct mg_connection *nc);
static void  event_handler                (struct mg_connection *nc, int ev, void *event_data);
static void  broadcast                    (struct mg_connection *nc, const struct mg_str msg);
static void* thread_read_serial_port      (void* param);
static void* thread_animation_side_by_side(void* param);
static BOOL  set_serial_port              (int serial_port_fd);
static BOOL  send_led_strip_cmd           (t_led_strip_vec led_strip_vec);
static BOOL  start_stop_thread_animation  (BOOL start);
static BOOL  add_sound_file               (const char* directory, const char* file_name);
static BOOL  start_tactile_button_handler (void);
static BOOL  init_sound                   (void);
static BOOL  beep                         (const char* cmd);
static BOOL  save_pattern                 (const char* msg);

#if DEMO_WITH_SOUND
//========================================================
// FIX ME
//========================================================
// These function are implemented in ../demo/tinyplay.c
// We should use an .h for their definition
//========================================================
int initSound    (void);
int addSoundFile (char *file);
int playSound    (int soundNum);
//========================================================

//#define TACT_BUTTON_PIN 29 // GPIO 21
#define TACT_BUTTON_PIN 21
static void handle_tactile_button_event(void)
{
    // TODO store the last position from the capsense bar and poke it in here (or just let the UI handle that)
    broadcast(s_network_connection, mg_mk_str("{\"event\":\"button\", \"id\":\"capybara\", \"action\":\"pup\"}"));
}// handle_tactile_button_event

static BOOL start_tactile_button_handler(void)
{
    wiringPiSetupGpio();
//    pthread_create(tbThreadId, NULL, &handle_tactile_button_event, NULL);
    pinMode(TACT_BUTTON_PIN, INPUT);
    pullUpDnControl(TACT_BUTTON_PIN, PUD_UP);
    wiringPiISR(TACT_BUTTON_PIN, INT_EDGE_SETUP, handle_tactile_button_event); // only supports button up right now, not press & hold

    return TRUE;
}// start_tactile_button_handler

static BOOL beep(const char* cmd)
{
    if (cmd == NULL) {
        printf("%s:%d, error: beeping soundi is NULL\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if (strstr(cmd, "slide") != NULL) {
        playSound(0);
    }
    else if (strstr(cmd, "peek") != NULL) {
        playSound(1);
    }
    else if (strstr(cmd, "select-preset") != NULL) {
        playSound(2);
    }
    else {
        printf("%s:%d, error: unsupported sound: %s\n", cmd);
        return FALSE;
    }

    return TRUE;
}// beep
#else // DEMO_WITH_SOUND

static BOOL beep(const char* cmd)
{
    printf("%s:%d, error: not compiled for demo, won't beed: %s\n", __FUNCTION__, __LINE__, cmd);
    return FALSE;
}// beep

static BOOL start_tactile_button_handler (void)
{
    printf("%s:%d, warning: won't start, not in build with DEMO_WITH_SOUND\n", __FUNCTION__, __LINE__);
    return TRUE;
}// start_tactile_button_handler

#endif // DEMO_WITH_SOUND

static BOOL add_sound_file(const char* directory, const char* file_name)
{
    char str_file_path[PATH_MAX];

    if ((directory == NULL) || (file_name == NULL))
    {
        printf("%s:%d, error: invalid parameter(s)", __FUNCTION__, __LINE__);
        return FALSE;
    }

    memset  (str_file_path, '\0', sizeof(str_file_path));
    snprintf(str_file_path, sizeof(str_file_path) - 1, "%s/%s", directory, file_name);

#if DEMO_WITH_SOUND
    return (addSoundFile(str_file_path) < 0) : FALSE ? TRUE;
#else
    printf("%s:%d, warning: file: %s won't be added, not build with DEMO_WITH_SOUND\n", __FUNCTION__, __LINE__, str_file_path);
    return TRUE;
#endif // DEMO_WITH_SOUND

}// add_sound_file

static BOOL init_sound(void)
{
#if DEMO_WITH_SOUND
    return (addSoundFile(str_file_path) < 0) : FALSE ? TRUE;
#else
    printf("%s:%d, warning: won't init sound, not build with DEMO_WITH_SOUND\n", __FUNCTION__, __LINE__);
    return TRUE;
#endif // DEMO_WITH_SOUND

}// init_sound

void sleep_ms(int milliseconds)
{
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}// sleep_ms

static BOOL start_stop_thread_animation(BOOL start)
{
    void* thread_ret_val = NULL;

    if (start == FALSE)
    {
        if (s_thread_animation_id == INT_MAX)
        {
            return TRUE;
        }

        printf("%s:%d, info: stopping animation...\n", __FUNCTION__, __LINE__);
        s_stop_animation_side_by_side = TRUE;
        pthread_join(s_thread_animation_id, &thread_ret_val);
        s_thread_animation_id = INT_MAX;
        return TRUE;
    }// if it's a stop

    if (s_thread_animation_id != INT_MAX)
    {
        return TRUE;
    }

    printf("%s:%d, info: starting animation...\n", __FUNCTION__, __LINE__);
    s_stop_animation_side_by_side = FALSE;
    if (pthread_create(&s_thread_animation_id, NULL, &thread_animation_side_by_side, NULL))
    {
        printf("%s:%d, error: failed to create thread\n", __FUNCTION__,  __LINE__);
        return FALSE;
    }

    return TRUE;
}// start_stop_thread_animation

static BOOL send_led_strip_cmd(t_led_strip_vec led_strip_vec)
{
    int  len_head = strlen (s_psoc_cmd_set_strip_head);
    int  len_tail = strlen (s_psoc_cmd_set_strip_tail);
    int  len_vec  = 0;
    char str_led_strip[256];

    memset(str_led_strip, '\0', sizeof(str_led_strip));

                         // LR-LG-LB-W1-W2-W3-W4-W5-W6-W7-W8-W9-10-11-12-13-RR-RG-RB
    sprintf(str_led_strip, "%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d",
            led_strip_vec[E_LED_LR ],
            led_strip_vec[E_LED_LG ],
            led_strip_vec[E_LED_LB ],
            led_strip_vec[E_LED_W1 ],
            led_strip_vec[E_LED_W2 ],
            led_strip_vec[E_LED_W3 ],
            led_strip_vec[E_LED_W4 ],
            led_strip_vec[E_LED_W5 ],
            led_strip_vec[E_LED_W6 ],
            led_strip_vec[E_LED_W7 ],
            led_strip_vec[E_LED_W8 ],
            led_strip_vec[E_LED_W9 ],
            led_strip_vec[E_LED_W10],
            led_strip_vec[E_LED_W11],
            led_strip_vec[E_LED_W12],
            led_strip_vec[E_LED_W13],
            led_strip_vec[E_LED_RR ],
            led_strip_vec[E_LED_RG ],
            led_strip_vec[E_LED_RB ]);
    len_vec = strlen (str_led_strip);

    if (write(s_serial_port_fd, s_psoc_cmd_set_strip_head, len_head) != len_head)
{
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if (s_show_set_strip)
    {
        printf("%s\n", str_led_strip);
    }

    if (write(s_serial_port_fd, str_led_strip, len_vec) != len_vec)
    {
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if (write(s_serial_port_fd, s_psoc_cmd_set_strip_tail, len_tail) != len_tail)
    {
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    return TRUE;
}// send_led_strip_cmd

static void* thread_animation_side_by_side(void* param)
{
    int                len_anim_disable      = strlen(s_psoc_cmd_animation_disable);
    int                len_anim_enable       = strlen(s_psoc_cmd_animation_enable );
    int                left_led              = E_LED_W1 ;
    int                right_led             = E_LED_W13;
    unsigned short int white_intensity       = MAX_LED_INTENSITY;
    BOOL               inward                = TRUE;
    t_led_strip_vec    led_strip_vec;

    UNUSED(param);

    if (write(s_serial_port_fd, s_psoc_cmd_animation_disable, len_anim_disable) != len_anim_disable)
    {
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    sleep(1);

#if 0
    int len_telemetry_disable = strlen(s_psoc_cmd_telemetry_disable);
    if (write(s_serial_port_fd, s_psoc_cmd_telemetry_disable, len_telemetry_disable) != len_telemetry_disable)
    {
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    sleep(1);
#endif

    for (int i = 0; i < E_LED_MAX; i++)
    {
        led_strip_vec[i] = 0;
    }

    if (send_led_strip_cmd(led_strip_vec) == FALSE)
    {
        printf("%s:%d, error: failed to set led strip\n", __FUNCTION__, __LINE__);
        goto end_of_animation;
    }

    s_stop_animation_side_by_side = FALSE;
    while (s_stop_animation_side_by_side == FALSE)
    {
        for (int i = 0; i < E_LED_MAX; i++)
        {
            led_strip_vec[i] = 0;
        }

#if 1
        if (left_led & 0x01)
        {
            led_strip_vec[E_LED_LR] = MAX_LED_INTENSITY;
            led_strip_vec[E_LED_LG] = 0                ;
            led_strip_vec[E_LED_LB] = 0                ;
            led_strip_vec[E_LED_RR] = MAX_LED_INTENSITY;
            led_strip_vec[E_LED_RG] = 0                ;
            led_strip_vec[E_LED_RB] = 0                ;
        }
        else
        {
            led_strip_vec[E_LED_LR] = 0                ;
            led_strip_vec[E_LED_LG] = MAX_LED_INTENSITY;
            led_strip_vec[E_LED_LB] = 0                ;
            led_strip_vec[E_LED_RR] = 0                ;
            led_strip_vec[E_LED_RG] = MAX_LED_INTENSITY;
            led_strip_vec[E_LED_RB] = 0                ;
        }
#endif

        led_strip_vec[left_led ] = white_intensity;
        led_strip_vec[right_led] = white_intensity;

        if (send_led_strip_cmd(led_strip_vec) == FALSE)
        {
            printf("%s:%d, error: failed to set led strip\n", __FUNCTION__, __LINE__);
            goto end_of_animation;
        }

        if (inward)
        {
            if (left_led >= E_LED_W6)
            {
                inward = FALSE;
            }
            else
            {
                left_led ++;
                right_led--;
            }
        }// if we are going inward
        else
        {
            if (left_led <= E_LED_W1)
            {
                inward = TRUE;
            }
            else
            {
                left_led --;
                right_led++;
            }
        }// else, we are going outward

        sleep_ms(1000/s_anim_frame_rate_per_sec);
    }// while it's not time to stop

end_of_animation:

#if 0
    int len_telemetry_enable = strlen(s_psoc_cmd_telemetry_enable);
    if (write(s_serial_port_fd, s_psoc_cmd_telemetry_enable, len_telemetry_enable) != len_telemetry_enable)
    {
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    sleep(1);
#endif

    if (write(s_serial_port_fd, s_psoc_cmd_animation_enable, len_anim_enable) != len_anim_enable)
    {
        printf("%s:%d, error: failed to write to serial port\n", __FUNCTION__, __LINE__);
    }
    sleep(1);

    return NULL;
}// thread_animation_side_by_side

static void* thread_read_serial_port (void* param)
{
    int  serial_port  = (int) param;
    char current_byte = 0x00;
    unsigned int  insert_pos   = 0;
    char current_str[2048];

    memset(current_str, '\0', sizeof(current_str));

    while (! s_stop_thread)
    {
        read(serial_port, &current_byte, 1);

        if (current_byte)
        {
            current_str[insert_pos++] = current_byte;
            if ((current_byte == s_end_of_cmd) || (insert_pos == sizeof(current_str)))
            {
                struct mg_str message = {current_str, insert_pos};
                broadcast(s_network_connection, message);
                memset(current_str, '\0', sizeof(current_str));
                insert_pos = 0;
            }
        }
    }// while we have

    return NULL;
}// thread_read_serial_port

static BOOL set_serial_port(int serial_port_fd)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);

    if (tcgetattr (serial_port_fd, &tty) != 0)
    {
        printf("%s:%d, error: can't get  serial port attribut\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    cfsetospeed(&tty, (speed_t)B115200);
    cfsetispeed(&tty, (speed_t)B115200);

    tty.c_cflag     &=  ~PARENB;        // 8N1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cc[VMIN]   =  1;              // read doesn't block
    tty.c_cc[VTIME]  =  5;              // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines

    cfmakeraw(&tty);

    tcflush(serial_port_fd, TCIFLUSH);
    if (tcsetattr (serial_port_fd, TCSANOW, &tty) != 0)
    {
        printf("%s:%d, error: can't get  serial port attribut\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    return TRUE;
}// set_serial_port

static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;

    write(s_serial_port_fd, s_psoc_cmd_telemetry_disable, strlen(s_psoc_cmd_telemetry_disable));
    printf("%s:%d, signal: %d\n", __FUNCTION__, __LINE__, sig_num);
}

static BOOL is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET ? TRUE : FALSE;
}

static void broadcast(struct mg_connection* network_connection, const struct mg_str msg)
{
    struct mg_connection* current_connection = NULL;
    //char                  buf [10000];//need to replace this with run time memory. this is inconvenient
    char*                 buf ;
    char                  addr[32];

    mg_sock_addr_to_str(&network_connection->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

    buf = malloc((int)msg.len +1) ;
    if (buf == NULL)
    {
        printf("%s, unable to allocate memory\n",__FUNCTION__);
	return;
    }
    //memset(buf, '\0', sizeof(buf));
    memset(buf, '\0', (int)msg.len + 1);
    strncpy(buf, msg.p, (int) msg.len);

    if (s_thread_animation_id == INT_MAX)
    {
        printf("%s", buf); // echo on the console

        if (buf[strlen(buf)-1] != '\n')
        {
            printf("\n");
        }// If the message is not ended by a new line, put one
        fflush (stdout);
    }// If the animation is not running

    for (current_connection  = mg_next(network_connection->mgr, NULL);
         current_connection != NULL                                  ;
         current_connection = mg_next(network_connection->mgr, current_connection))
    {
        if (current_connection == network_connection)
        {
            //continue; // skip this one, don't send to ourself
        }
        mg_send_websocket_frame(current_connection, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }// for all the connection
    if (buf)
        free(buf);
}// broadcast

static void event_handler(struct mg_connection* network_connection, int event, void *event_data)
{
    struct http_message* http_msg = (struct http_message*) event_data;

    switch (event)
    {
        case MG_EV_HTTP_REQUEST:
        {
            // serve the index.html page
            mg_serve_http(network_connection, http_msg, s_http_server_opts);
            break;
        }// MG_EV_HTTP_REQUEST

        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            // new websocket connection, tell everyone
            broadcast(network_connection, mg_mk_str("++ joined"));
            break;
        }// MG_EV_WEBSOCKET_HANDSHAKE_DONE

        case MG_EV_WEBSOCKET_FRAME:
        {
            // new websocket message, tell everyone
            struct websocket_message* web_socket_msg = (struct websocket_message *) event_data;
            struct mg_str             msg_str        = {(char *) web_socket_msg->data, web_socket_msg->size};

            if ((web_socket_msg->size) && (web_socket_msg->data[web_socket_msg->size - 1]) == s_end_of_cmd)
            {
                char str[1024];

                memset (str, '\0', sizeof(str));
                strncpy(str, msg_str.p, msg_str.len);
                printf ("%s:%d, writing command: %s\n", __FUNCTION__, __LINE__, str);
                unsigned int nb_written = write(s_serial_port_fd, web_socket_msg->data, web_socket_msg->size);

                if (nb_written != web_socket_msg->size)
                {
                    printf("%s:%d, error: failed to write: %u characters on the serial port, written: %u\n", __FUNCTION__, __LINE__, (unsigned int) web_socket_msg->size, nb_written);
                }
            }// If it's a command to the PSoC
            else
            {
                if (strncasecmp(msg_str.p, s_start_anim, msg_str.len) == 0)
                {
                    printf("%s:%d, log: starting animation\n", __FUNCTION__, __LINE__);
                    if (start_stop_thread_animation(TRUE) == FALSE)
                    {
                        printf("%s:%d, error: failed to start animation\n", __FUNCTION__, __LINE__);
                    }

                }// if we are starting the animation
		        else if(strstr(msg_str.p, "\"beep\"") != NULL) {
		            if (beep(msg_str.p) == FALSE)
                    {
                        printf("%s:%d, error: failed to beep\n", __FUNCTION__, __LINE__);
                    }
		        }// else if we are doing a beep
                else if (strncasecmp(msg_str.p, s_stop_anim, msg_str.len) == 0)
                {
                    printf("%s:%d, log: stopping animation\n", __FUNCTION__, __LINE__);
                    if (start_stop_thread_animation(FALSE) == FALSE)
                    {
                        printf("%s:%d, error: failed to stop animation\n", __FUNCTION__, __LINE__);
                    }
                }// else, if we are stopping the animation
                else if (strstr (msg_str.p,s_save_pattern) != 0)
                {
                    printf("%s:%d, log: saving pattern\n", __FUNCTION__, __LINE__);
                    //search for existing file name, if exists, clear it and replace data with new data or else create a new file and update data
                     
                }// if command is to save pattern
		else if (strstr (msg_str.p,s_get_configuration) != 0)
		{
		    printf("%s:%d, log: get lightbar configuration \n", __FUNCTION__,__LINE__);
		    char *config_name = strtok(msg_str.p,",");
		    std::string name;
		    if (config_name == NULL)
			return;
		    config_name = strtok(NULL,",");
		    if (config_name == NULL)
			return;
                    printf("config file name = %s",config_name);
		    std::string config = CLightBarAnimation::getLightBarConfig (static_cast<std::string>(config_name));
                    std::string json_msg = "{\"event\":\"Configuration\",\"Config\":"+config+"}";
		    printf("json msg = %s\n",json_msg.c_str());
		    broadcast (network_connection,mg_mk_str(json_msg.c_str()));
		
		}// get light bar composition i.e. RGBW configuration and postion
                else if (strstr (msg_str.p,s_get_animation) != 0)
                {
                    printf("%s:%d, log: get animation \n", __FUNCTION__, __LINE__);
		    std::string json_data = CLightBarAnimation::getAllAnimations();
		    //std::cout << "json data - " << json_data << std::endl;
		    //send it back to client?? 
		    broadcast(network_connection, mg_mk_str(json_data.c_str()));
                }// if command is to return a particular pattern
                else if (strstr (msg_str.p,s_play_animation) != 0)
                {
                    printf("%s:%d, log: playing animation\n", __FUNCTION__, __LINE__);
		    CLightBarAnimation lb_anim;
		    char *anim = strtok(msg_str.p,",");
		    std::string anim_name;
		    if (anim == NULL)
			return;
		    anim = strtok(NULL,",");
		    if (anim == NULL)
			return;
		    anim_name = static_cast <std::string> (anim);
		    std::cout << "playing animation " << anim_name << std::endl;

		    lb_anim.setCommunicationfd(s_serial_port_fd);
		    //call getAnimation, parse animation patterns, send it to PSOC for playing, maintain time requirements
		    lb_anim.playAnimation(anim_name);
                    
                }// if command is to return a particular pattern
                else if (strstr (msg_str.p,s_save_animation) != 0)
		{
                    printf("%s:%d, log: saving animation\n", __FUNCTION__, __LINE__);
		    //call function to save animation
		    //
		    printf("json data - %s",msg_str.p);
		    char *anim = strtok(msg_str.p,"&");
		    
		    std::string anim_name;
		    if (anim == NULL)
			return;
		    anim = strtok(NULL,"&");
		    if (anim == NULL)
			return;
		    //anim_name = static_cast <std::string> (anim);
		    
		    std::cout << "saving animation - " << anim << std::endl;
		    bool success = CLightBarAnimation::saveExistingAnimation (anim);
		    if (success)
			broadcast(s_network_connection, mg_mk_str("{\"event\":\"action success\", \"id\":\"\", \"action\":\"\",\"message\":\"add animation\"}"));
		    else
			broadcast(s_network_connection, mg_mk_str("{\"event\":\"action error\", \"id\":\"\", \"action\":\"\",\"error\":\"duplicate\"}"));
		        
		}
                else if (strstr (msg_str.p,s_delete_animation) != 0)
		{

                    printf("%s:%d, log: delete animation\n", __FUNCTION__, __LINE__);
		    CLightBarAnimation lb_anim;
		    char *anim = strtok(msg_str.p,",");
		    std::string anim_name;
		    if (anim == NULL)
			return;
		    anim = strtok(NULL,",");
		    if (anim == NULL)
			return;
		    std::cout << "delete animation " << anim << std::endl;

		    //call getAnimation, parse animation patterns, send it to PSOC for playing, maintain time requirements
		    lb_anim.deleteAnimation(anim);

		}
                else if (strncasecmp(msg_str.p, s_set_rate_anim, strlen(s_set_rate_anim)) == 0)
                {
                    unsigned int anim_frame_rate = atoi(msg_str.p + strlen(s_set_rate_anim) + 1);

                    if ((anim_frame_rate == 0) || (anim_frame_rate >= 1000))
                    {
                        printf("%s:%d, error: invalid animation frame rate: %s\n", __FUNCTION__, __LINE__, msg_str.p + strlen(s_set_rate_anim) + 1);
                        break;
                    }

                    printf("%s:%d, log: setting frame rate: %u\n", __FUNCTION__, __LINE__, anim_frame_rate);

                    s_anim_frame_rate_per_sec = anim_frame_rate;
                }// else, if we are setting the animation frame rate
                else if (strncasecmp(msg_str.p, s_enable_show_set_strip, strlen(s_enable_show_set_strip)) == 0)
                {
                    printf("%s:%d, log: enabling show set strip\n", __FUNCTION__, __LINE__);
                    s_show_set_strip = TRUE;
                }// else, if we are enabling the log of set strip
                else if (strncasecmp(msg_str.p, s_disable_show_set_strip, strlen(s_disable_show_set_strip)) == 0)
                {
                    printf("%s:%d, log: disabling show set strip\n", __FUNCTION__, __LINE__);
                    s_show_set_strip = FALSE;
                }// else, if we are disabling the log of set strip
                else
                {
                    printf("%s:%d, error: unsupported command: %s\n", __FUNCTION__, __LINE__, msg_str.p);
                }// else, the command is unsupported
            }// else, it's a command to mongoose

            broadcast(network_connection, msg_str);
            break;
        }// MG_EV_WEBSOCKET_FRAME

        case MG_EV_CLOSE:
        {
            // disconnect, tell everyone.
            if (is_websocket(network_connection))
            {
                broadcast(network_connection, mg_mk_str("-- left"));
            }
            break;
        }// MG_EV_CLOSE
    }// switch on event

}// event_handler

int main(int argc, char *argv[])
{
    pthread_t     thread_id        = INT_MAX;
    char*         dir_separator    = NULL;
    void*         thread_ret_val   = NULL;
    const char*   serial_port_name = NULL;
    struct mg_mgr mgr;

    signal (SIGTERM, signal_handler);
    signal (SIGINT , signal_handler);

    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    if (init_sound() == FALSE)
    {
        printf("%s:%d, error: failed to initialize the sound device...exiting\n", __FUNCTION__, __LINE__);
        return -1;
    }

    if ((add_sound_file(s_sound_file_directory, s_sound_file_slide        ) == FALSE) || // keep this order, see beep function
        (add_sound_file(s_sound_file_directory, s_sound_file_peek         ) == FALSE) ||
        (add_sound_file(s_sound_file_directory, s_sound_file_select_preset) == FALSE)  )
    {
        printf("%s:%d, error: failed to add one of the required sound file...exiting\n", __FUNCTION__, __LINE__);
        return -2;
    }

    if (start_tactile_button_handler() == FALSE)
    {
        printf("%s:%d, error: failed to start the tactile button handler...exiting\n", __FUNCTION__, __LINE__);
        return -3;
    }

    mg_mgr_init(&mgr, NULL);

    if ((argc > 0) && ((dir_separator = strrchr(argv[0], DIRSEP)) != NULL))
    {
        *dir_separator                   = '\0';
        s_http_server_opts.document_root = argv[0];// set the document root the application path
    }// If we found a directory in the application path file name

    serial_port_name = (argc > 1) ? argv[1] :  s_default_serial_port_name;

    s_network_connection = mg_bind(&mgr, s_http_port, event_handler);
    mg_set_protocol_http_websocket(s_network_connection);

    s_serial_port_fd = open(serial_port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (s_serial_port_fd < 0)
    {
        printf("%s:%d, error: unable to open serial port: %s\n", __FUNCTION__, __LINE__, serial_port_name);
        return -4;
    }
    fcntl(s_serial_port_fd, F_SETFL, 0);

    if (set_serial_port(s_serial_port_fd) == FALSE)
    {
        printf("%s:%d, error: failed to set serial port: %s\n", __FUNCTION__, __LINE__, serial_port_name);
        return -5;
    }

    if (write(s_serial_port_fd, s_psoc_cmd_telemetry_enable, strlen(s_psoc_cmd_telemetry_enable)) != (ssize_t) strlen(s_psoc_cmd_telemetry_enable))
    {
        printf("%s:%d, error: failed to write to serial port: %s\n", __FUNCTION__, __LINE__, serial_port_name);
        return -6;
    }

    if (pthread_create(&thread_id, NULL, &thread_read_serial_port, (void*) s_serial_port_fd))
    {
        printf("%s:%d, error: failed to create read serial thread\n", __FUNCTION__, __LINE__);
        close(s_serial_port_fd);
        return -7;
    }

    s_signal_received = 0;
    printf("================================================\n");
    printf("http server is listening on port: %s\n", s_http_port);
    printf("PSoC serial interface is on port: %s\n", serial_port_name);
    printf("================================================\n");
    while (s_signal_received == 0)
    {
        mg_mgr_poll(&mgr, 200);
    }

    printf("exiting...\n");

    s_stop_thread = TRUE;

    if (start_stop_thread_animation(FALSE) == FALSE)
    {
        printf("%s:%d, warning: failed to stop animation thread\n", __FUNCTION__, __LINE__);
    }// if we failed to stop the animation thread

    pthread_cancel(thread_id);
    pthread_join  (thread_id, &thread_ret_val);

    mg_mgr_free(&mgr);

    if (s_serial_port_fd != -1)
    {
        write(s_serial_port_fd, s_psoc_cmd_telemetry_disable, strlen(s_psoc_cmd_telemetry_disable));
        close(s_serial_port_fd);
    }

    return 0;
}// main
