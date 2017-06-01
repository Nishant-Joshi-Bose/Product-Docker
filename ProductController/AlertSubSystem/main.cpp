#include <iostream>
#include <time.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <string.h>
//#include "AlertsScheduler.h"
#include "AlertsSystemClient.h"
#include "AlsaAlertOutput.h"
#include "mongoose.h"
#include "AlertsMessage.h"
//#incllue "AlertsClient.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>

using namespace rapidjson;

int parseMsg (char *);

struct mg_mgr mgr;
struct mg_connection *nc;

static const char*  s_http_port            = "8010";
static struct mg_serve_http_opts s_http_server_opts;

//static 	struct tm *t_time = nullptr;
static  std::string a_type = "";
static  std::string alert_command = "NONE";
static  std::string alert_id = "";
static  std::string a_schtime = "";

static sig_atomic_t s_signal_received = 0;
static void reply (struct mg_connection *nc, std::string a_id);

static void broadcast(struct mg_connection *ncc, const struct mg_str msg) ;

void test_callback (std::string alert_id)
{
	std::cout << "test_callback - alert id " << alert_id << std::endl;

	reply(nc,alert_id);
}

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);  // Reinstantiate signal handler
  s_signal_received = sig_num;
}

static int is_websocket(const struct mg_connection *nc) {
  return nc->flags & MG_F_IS_WEBSOCKET;
}

static void broadcast(struct mg_connection *ncc, const struct mg_str msg) {
  struct mg_connection *c;
  char buf[500];
  //char addr[32];

  snprintf(buf, sizeof(buf), "%.*s", (int) msg.len, msg.p);
  printf("%s\n", buf); /* Local echo. */
    //PJ - added to parse input message
    if (parseMsg (buf))
	//set alert and send response to client	
    
    for (c = mg_next(ncc->mgr, NULL); c != NULL; c = mg_next(ncc->mgr, c)) {
	if (c == ncc) {
		mg_send_websocket_frame (c,WEBSOCKET_OP_TEXT,buf,strlen(buf));
		break;
	}
    }
}

static void reply (struct mg_connection *ncc, std::string a_id)
{
  struct mg_connection *c;
  char buf[50];

  //copy alert_id to buf
  strcpy (buf,a_id.c_str());

    for (c = mg_next(ncc->mgr, NULL); c != NULL; c = mg_next(ncc->mgr, c)) {
	//if (c == ncc) {
	if (c != ncc) {
		mg_send_websocket_frame (c,WEBSOCKET_OP_TEXT,buf,strlen(buf));
		break;
	}
    }

}

static void ev_handler(struct mg_connection *ncc, int ev, void *ev_data) {

  struct http_message* http_msg = (struct http_message*) ev_data;

  switch (ev) {
       case MG_EV_HTTP_REQUEST:
        {
            // serve the index.html page
            mg_serve_http(ncc, http_msg, s_http_server_opts);
            break;
        }// MG_EV_HTTP_REQUEST
	break;

    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
      /* New websocket connection. Tell everybody. */
      // broadcast(nc, mg_mk_str("++ joined"));
	std::cout << "connected with client..." << std::endl;
      break;
    }
    case MG_EV_WEBSOCKET_FRAME: {
      struct websocket_message *wm = (struct websocket_message *) ev_data;
      /* New websocket message. Tell everybody. */
      struct mg_str d = {(char *) wm->data, wm->size};
      // PJ - <TBD> ideally the message received should be queued here for message handler to deque and parse 
      broadcast(ncc, d);
      break;
    }
    case MG_EV_CLOSE: {
      /* Disconnect. Tell everybody. */
      if (is_websocket(nc)) {
        //broadcast(nc, mg_mk_str("-- left"));
	std::cout << "disconnected with client..." << std::endl;
      }
      break;
    }
  }
}


int parseMsg(char *Msg)
{
#if 0
	int timer_time = -1;
	std::ostringstream os ;
	os << Msg; 

	std::string MsgStr = os.str();	
	char *tok = strtok(Msg, ",");
	if (tok != nullptr)
	{
		if(0 ==  strcmp(tok,"TIMER"))
		{
			a_type = "TIMER";
			tok = strtok(NULL,",");
			if (tok != nullptr)
			{
				timer_time =atoi(tok);
				if (timer_time > 0) {
					//read from command line to schedule a new time
					time_t rawtime;
					time(&rawtime);
					rawtime += timer_time;
					t_time= gmtime (&rawtime);
					alert_command = "SETALERT";
				}	
			}
		}
		else if(0 ==  strcmp(tok,"ALARM"))
		{
			tok = strtok(NULL,":");
			if (tok != nullptr)
			{
				int hour = atoi (tok);
				time_t rawtime;
				time(&rawtime);
				t_time= localtime (&rawtime);
			 	if (hour > t_time->tm_hour && hour < 24)
				{
					t_time->tm_hour = hour;		
				}		
				tok = strtok (NULL,":");
				if (tok != nullptr)
				{
					int min = atoi (tok);
					if (min >=0 && min < 60)
						t_time->tm_min = min;
					t_time->tm_sec = 0;
				}
				rawtime = timelocal (t_time);
				t_time = gmtime (&rawtime);
				a_type = "ALARM";
				alert_command = "SETALERT";
			}


		}
		else if (0 == strcmp(tok,"DELETE"))
		{
			tok = strtok (NULL,",");
			if (tok != nullptr)
			{
				alert_id = static_cast <std::string> (tok);
				alert_command = "DELETEALERT";
			}
		}
		else 
		{
			std::cout << "command not supported"<< std::endl;
			return 0;
		}
	}
#else //json parsing

    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

    // In-situ parsing, decode strings directly in the source string. Source must be string.
    char buffer[1000];
    memcpy(buffer, Msg, strlen(Msg)+1);
    if (document.ParseInsitu(buffer).HasParseError())
        return 1;

    printf("\nParsing to document succeeded.\n");

    assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

    assert(document.HasMember("alert_type"));
    assert(document["alert_type"].IsString());
    printf("alert_type = %s\n", document["alert_type"].GetString());

    // Since version 0.2, you can use single lookup to check the existing of member and its value:
    Value::MemberIterator hello = document.FindMember("alert_type");
    assert(hello != document.MemberEnd());
    assert(hello->value.IsString());
    if (strcmp("TIMER", hello->value.GetString()) == 0) {
	a_type = "TIMER";
	alert_command = "SETALERT";
        (void)hello;
    }
    else if (strcmp("ALARM", hello->value.GetString()) == 0) {
	a_type = "ALARM";
	alert_command = "SETALERT";
        (void)hello;


    }
    else if(strcmp("DELETE", hello->value.GetString()) == 0) {

	alert_command = "DELETEALERT";
        assert(document["alert_id"].IsString());   
        printf("alert_id = %s\n", document["alert_id"].GetString()); // Alternative (int)document["i"]
        alert_id = document["alert_id"].GetString();
	return 1;
    }

        assert(document["schedule_time"].IsString());   
        printf("scheduled_time = %s\n", document["schedule_time"].GetString()); // Alternative (int)document["i"]
        a_schtime = document["schedule_time"].GetString();
#if 0
        int timer_time = document["schedule_time"].GetInt();
        if (timer_time > 0) {
	    //read from command line to schedule a new time
	    time_t rawtime;
	    time(&rawtime);
	    rawtime += timer_time;
	    t_time= gmtime (&rawtime);
	}
#endif
    

#endif

	return 1;
}

void addAlert (CAlertsSystemClient* m_pAlerts_client, CAlertsMessage newMessage)
{
	std::ostringstream oss;
	//oss  << std::put_time(t_time,"%FT%T");
	//newMessage.setScheduledTime (oss.str());
	newMessage.setScheduledTime (a_schtime);
	//PJ - <TBD> implement logic to find alert type from parse message.
	//easy way could be making it static 
	newMessage.setAlertType(a_type);
	std::cout<< "new alert scheduled time" << newMessage.getScheduledTime()<<std::endl;
	m_pAlerts_client->addAlert(newMessage);

}


void deleteAlert (CAlertsSystemClient *client, std::string alert_id)
{
	client->deleteAlert (alert_id);
}

int main(int argc, char ** argv) {

  char*         dir_separator    = NULL;

  //read from command line to schedule a new time
  CAlertsSystemClient* m_pAlerts_client = new CAlertsSystemClient (IL::CreateTask("AlertsSystemClient"));

  //register observer for the active alert
  CAlsaAlertOutput alsaOutput (m_pAlerts_client,IL::CreateTask("AlsaPlayback"));
  m_pAlerts_client->registerSource ("web",test_callback);

  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  setvbuf(stdout, NULL, _IOLBF, 0);
  setvbuf(stderr, NULL, _IOLBF, 0);

  mg_mgr_init(&mgr, NULL);
  if ((argc > 0) && ((dir_separator = strrchr(argv[0], DIRSEP)) != NULL))
  {
      *dir_separator                   = '\0';
      s_http_server_opts.document_root = argv[0];// set the document root the application path
  }// If we found a directory in the application path file name


  nc = mg_bind(&mgr, s_http_port, ev_handler);

  mg_set_protocol_http_websocket(nc);

  printf("Started on port %s\n", s_http_port);
  while (s_signal_received == 0) {
    mg_mgr_poll(&mgr, 200);
	if (alert_command == "SETALERT")
	{
		std::cout << "scheduling alert - " << std::endl;
		CAlertsMessage newMessage ;
		addAlert (m_pAlerts_client, newMessage);
		alert_command = "";
	}
	else if (alert_command == "DELETEALERT")
	{
		deleteAlert (m_pAlerts_client, alert_id);
		alert_command = "";
	}
  }
  mg_mgr_free(&mgr);

  return 0;
}

