#pragma once

#include <iostream>
#include "AsyncCallback.h"
#include "APTaskFactory.h"
#include "BreakThread.h"
#include "AlertsMessage.h"

class CAlertsManagerAPI {
public:
	//TBD - need to change the API to make it more generic
	virtual bool addAlert(CAlertsMessage &alertMessage) = 0;

	//TBD - may need to change the API to make it more generic
	virtual bool deleteAlert (std::string &alert_id) = 0;

	//This function returns alert id, that needs to shared
	//with the module that requested alert. For AlertSubsystem
	//alert id is used for identifyication of scheduled alerts
	//and to cancel/delete alert
	virtual void reportAlertID (CAlertsMessage &) = 0;
	
	//Notify active alert
	virtual void notifyActiveAlert (std::string) = 0;
};
