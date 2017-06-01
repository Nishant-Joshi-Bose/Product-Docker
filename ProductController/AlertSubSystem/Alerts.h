#pragma once

#include <iostream>
#include <time.h>
#include <uuid/uuid.h>
#include "APTimer.h"
#include "APTimerFactory.h"
#include "APTask.h"

#ifdef NATIVE
#include <ifstream>
#include <ofstream>
#endif


typedef enum _alert_type{
	INVALID = 0,
	TIMER,
	ALARM,
}alert_type;

typedef enum _alert_source{
	LOCAL = 0,
	AVS = 1
}alert_source;

class CAlerts {
public:
	CAlerts (NotifyTargetTaskIF *);  
	~CAlerts () ;
	std::string addAlert (struct tm *sch_time,std::function <void(std::string)> cbk);
	void callback_cbk();
	std::string getMessageId();
	bool deleteAlert ();
	std::string getAlertId ();
        time_t getScheduledUTCTime ();
private:
	std::string alert_id;  //The id will be returned to the module on successful alert/alarm setup
	std::string message_id;
	std::string message_token;
	struct tm scheduled_time;
	int t_time;
	alert_type  a_type;
	alert_source a_source;
	NotifyTargetTaskIF *m_alert_task;
	APTimerPtr m_alert_timer;
	std::function<void(std::string)> alert_callback;

	//bool removeAlertfromNVM ();
	//bool addAlerttoNVM ();
};


