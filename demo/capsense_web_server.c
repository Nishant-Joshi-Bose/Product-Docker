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
#include <wiringPi.h>

#include "mongoose.h"

// From the tinyplay stuff
int initSound(void);
int addSoundFile(char *file);
int playSound(int soundNum);

#ifndef BOOL
#define BOOL unsigned char
#endif // BOOL

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif // TRUE

static const char*               s_default_serial_port_name = "/dev/ttyUSB1";
static sig_atomic_t              s_signal_received          = 0;
static const char*               s_http_port                = "8001";
static struct mg_connection*     s_network_connection       = NULL;
static int                       s_serial_port_fd           = -1;
static BOOL                      s_stop_thread              = FALSE;
static char                      s_end_of_cmd               = '}';
static struct mg_serve_http_opts s_http_server_opts;

extern void  cfmakeraw              (struct termios *__termios_p); // this is hidden from termios.h
static BOOL  is_websocket           (const struct mg_connection *nc);
static void  event_handler          (struct mg_connection *nc, int ev, void *event_data);
static void  broadcast              (struct mg_connection *nc, const struct mg_str msg);
static void* thread_read_serial_port(void* param);
static BOOL  set_serial_port        (int serial_port_fd);

static void* thread_read_serial_port (void* param)
{
    int  serial_port  = (int) param;
    char current_byte = 0x00;
    int  insert_pos   = 0;
    char current_str[1024];

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
        printf("Error: can't get  serial port attribut\n");
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
        printf("Error: can't set serial port attribut\n");
        return FALSE;
    }

    return TRUE;
}// set_serial_port

//#define TACT_BUTTON_PIN 29 // GPIO 21
#define TACT_BUTTON_PIN 21
static void handle_tactile_button_event(void)
{
  // TODO store the last position from the capsense bar and poke it in here (or just let the UI handle that)
  broadcast(s_network_connection, mg_mk_str("{\"event\":\"button\", \"id\":\"capybara\", \"action\":\"pup\"}"));
}

static void start_tactile_button_handler(void)
{
    wiringPiSetupGpio();
//    pthread_create(tbThreadId, NULL, &handle_tactile_button_event, NULL);
    pinMode(TACT_BUTTON_PIN, INPUT);
    pullUpDnControl(TACT_BUTTON_PIN, PUD_UP);
    wiringPiISR(TACT_BUTTON_PIN, INT_EDGE_SETUP, handle_tactile_button_event); // only supports button up right now, not press & hold
}

static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;
}

static BOOL is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET ? TRUE : FALSE;
}

static void beep(char* cmd)
{
  printf("Beep ");
  if (strstr(cmd, "slide") != NULL) {
    printf("slide\n");
    playSound(0);
  }
  else if (strstr(cmd, "peek") != NULL) {
    printf("peek\n");
    playSound(1);
  }
  else if (strstr(cmd, "select-preset") != NULL) {
    printf("select-preset\n");
    playSound(2);
  }
  else {
    printf("no such sound %s\n", cmd);
  }
}

static void broadcast(struct mg_connection* network_connection, const struct mg_str msg)
{
    struct mg_connection* current_connection = NULL;
    char                  buf [500];
    char                  addr[32];

    mg_sock_addr_to_str(&network_connection->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

    //snprintf(buf, sizeof(buf), "%s %.*s", addr, (int) msg.len, msg.p);

    memset(buf, '\0', sizeof(buf));
    strncpy(buf, msg.p, (int) msg.len);

    printf("%s", buf); // echo on the console

    if (buf[strlen(buf)-1] != '\n')
    {
        printf("\n");
    }// If the message is not ended by a new line, put one
    fflush (stdout);

    for (current_connection  = mg_next(network_connection->mgr, NULL);
         current_connection != NULL                                  ;
         current_connection = mg_next(network_connection->mgr, current_connection))
    {
        if (current_connection == network_connection)
        {
            continue; // skip this one, don't send to ourself
        }
        mg_send_websocket_frame(current_connection, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }// for all the connection

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

                memset(str, '\0', sizeof(str));
                strncpy(str, (char*) web_socket_msg->data, (size_t) web_socket_msg->size);
		if (strstr(str, "\"beep\"") != NULL) {
		  beep(str);
		}
		else {
		  printf("%s:%d, writing command: %s\n", __FUNCTION__, __LINE__, str);
		  unsigned int nb_written = write(s_serial_port_fd, web_socket_msg->data, web_socket_msg->size);

		  if (nb_written != web_socket_msg->size)
		  {
		      printf("error: failed to write: %u characters on the serial port, written: %u\n", (unsigned int) web_socket_msg->size, nb_written);
		  }
		}
            }

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
    pthread_t     serial_thread_id        = -1;
    //pthread_t     tactbutton_thread_id        = -1;
    char*         dir_separator    = NULL;
    void*         thread_ret_val   = NULL;
    const char*   serial_port_name = NULL;
    struct mg_mgr mgr;

    signal (SIGTERM, signal_handler);
    signal (SIGINT , signal_handler);

    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    // Setup sound
    if (initSound() < 0) {
      exit(0);
    }
    if (addSoundFile("/home/pi/Eddie-Demo/capsense/sounds/slide.raw") < 0) {
      exit(0);
    }
    if (addSoundFile("/home/pi/Eddie-Demo/capsense/sounds/peek.raw") < 0) {
      exit(0);
    }
    if (addSoundFile("/home/pi/Eddie-Demo/capsense/sounds/select-preset.raw") < 0) {
      exit(0);
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
        printf("error: unable to open serial port: %s\n", serial_port_name);
        return -1;
    }
    fcntl(s_serial_port_fd, F_SETFL, 0);

    if (set_serial_port(s_serial_port_fd) == FALSE)
    {
        printf("error: failed to set serial port: %s\n", serial_port_name);
        return -2;
    }

    pthread_create(&serial_thread_id, NULL, &thread_read_serial_port, (void*) s_serial_port_fd);
    start_tactile_button_handler();

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
    pthread_cancel(serial_thread_id);
    pthread_join  (serial_thread_id, &thread_ret_val);
//    pthread_cancel(tactbutton_thread_id);
//    pthread_join  (tactbutton_thread_id, &thread_ret_val);

    mg_mgr_free(&mgr);

    if (s_serial_port_fd != -1)
    {
        close (s_serial_port_fd);
    }

    return 0;
}// main
