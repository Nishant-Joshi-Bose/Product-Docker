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

#include "mongoose.h"

#ifndef BOOL
#define BOOL unsigned char
#endif // BOOL

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif // TRUE

static const char*               s_default_serial_port_name = "/dev/ttyACM0";
static sig_atomic_t              s_signal_received          = 0;
static const char*               s_http_port                = "8000";
static struct mg_connection*     s_network_connection       = NULL;
static int                       s_serial_port_fd           = -1;
static BOOL                      s_stop_thread              = FALSE;
static struct mg_serve_http_opts s_http_server_opts;

extern void  cfmakeraw              (struct termios *__termios_p); // this is hidden from termios.h
static BOOL  is_websocket           (const struct mg_connection *nc);
static void  event_handler             (struct mg_connection *nc, int ev, void *event_data);
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
            if ((current_byte == '\n') || (insert_pos == sizeof(current_str)))
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

    cfsetospeed(&tty, (speed_t)B57600);
    cfsetispeed(&tty, (speed_t)B57600);

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

static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;
}

static BOOL is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET ? TRUE : FALSE;
}

static void broadcast(struct mg_connection* network_connection, const struct mg_str msg)
{
    struct mg_connection* current_connection = NULL;
    char                  buf [500];
    char                  addr[32];

    mg_sock_addr_to_str(&network_connection->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

    snprintf(buf, sizeof(buf), "%s %.*s", addr, (int) msg.len, msg.p);
    printf("%s\n", buf); /* Local echo. */

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

static void event_handler(struct mg_connection* network_connection, int ev, void *event_data)
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
            struct websocket_message *wm = (struct websocket_message *) event_data;
            struct mg_str d = {(char *) wm->data, wm->size};

            if (wm->data[wm->size - 1] == '!')
            {
                unsigned int nb_write = write(s_serial_port_fd, wm->data, wm->size);

                if (nb_write != wm->size)
                {
                    printf("error: failed to write: %u characters on the serial port, written: %u\n", (unsigned int) wm->size, nb_write);
                }
            }// If it's a command to the PSoC

            broadcast(network_connecction, d);
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
    pthread_t     thread_id      = -1;
    char*         dir_separator  = NULL;
    void*         thread_ret_val = NULL;
    struct mg_mgr mgr;

    signal (SIGTERM, signal_handler);
    signal (SIGINT , signal_handler);

    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    mg_mgr_init(&mgr, NULL);

    if ((argc > 0) && ((dir_separator = strrchr(argv[0], DIRSEP)) != NULL))
    {
        *dir_separator                   = '\0';
        s_http_server_opts.document_root = argv[0];// set the document root the application path
    }// If we found a directory in the application path file name

    s_network_connection = mg_bind(&mgr, s_http_port, event_handler);
    mg_set_protocol_http_websocket(s_network_connection);

    s_serial_port_fd = open(s_default_serial_port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (s_serial_port_fd < 0)
    {
        printf("error: unable to open serial port: %s\n", s_default_serial_port_name);
        return -1;
    }
    fcntl(s_serial_port_fd, F_SETFL, 0);

    if (set_serial_port(s_serial_port_fd) == FALSE)
    {
        printf("error: failed to set serial port: %s\n", s_default_serial_port_name);
        return -2;
    }

    pthread_create(&thread_id, NULL, &thread_read_serial_port, (void*) s_serial_port_fd);

    printf("started on port: %s\n", s_http_port);
    while (s_signal_received == 0)
    {
        mg_mgr_poll(&mgr, 200);
    }
    printf("exiting...\n");

    s_stop_thread = TRUE;
    pthread_cancel(thread_id);
    pthread_join  (thread_id, &thread_ret_val);

    mg_mgr_free(&mgr);

    if (s_serial_port_fd != -1)
    {
        close (s_serial_port_fd);
    }

    return 0;
}// main
