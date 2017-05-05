#pragma once

#include <iostream>
#include <functional>
#include "AlertsManagerAPI.h"
#include "AlertsScheduler.h"
#include "AlertsSystemClient.h"
#include "DPrint.h"

#define MAX_SCHEDULED_ALERTS (5)

class CAlertsSystemClient;

class CAlertsManager : public CAlertsManagerAPI {
public :
	CAlertsManager ();
	CAlertsManager (CAlertsSystemClient *);
	~CAlertsManager ();

	inline NotifyTargetTaskIF * getTask () const
	{
		return m_pTask;
		//return dynamic_cast <NotifyTargetTaskIF *> (m_pTask);
	}
	bool addAlert (CAlertsMessage &alertMessage);
	bool deleteAlert (std::string &alert_id);
	void reportAlertID (CAlertsMessage &);
	void notifyActiveAlert (std::string);
	//std::function <CAlertsMessage (std::string alert_id)> mgr_cbk;
private:
	bool addAlertAction (CAlertsMessage &alertMessage);
	void reportAlertIDAction (CAlertsMessage);
	void notifyActiveAlertAction (std::string);
	std::tm get_Time (std::string &);
	NotifyTargetTaskIF * m_pTask;
	CAlertsScheduler m_alertsScheduler;
	CAlertsSystemClient * m_alert_client;
	int scheduledAlerts;
	int activeAlerts;
	DPrint *m_plogger;
};

