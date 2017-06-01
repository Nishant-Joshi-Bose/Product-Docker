#pragma once
#include <iostream>
#include <vector>
#include "Alerts.h"



class CAlertsScheduler {
public:
	CAlertsScheduler (){}; 
	CAlertsScheduler (NotifyTargetTaskIF * m_task) : m_ap_task(m_task){}; 
	~CAlertsScheduler () {};

	// create notify task, a call back function (common), Star alert
	std::string addAlert (struct tm* t_time,std::function <void (std::string)> cbk);
	bool deleteAlert (std::string alert_mesage_id);
private:
	bool checkAlertExist (struct tm *);
	//list of alerts, contains APTimer and APTask object
	std::vector <CAlerts> m_alerts;
	std::vector <CAlerts>::iterator m_alerts_it;
	NotifyTargetTaskIF * m_ap_task;

};	
