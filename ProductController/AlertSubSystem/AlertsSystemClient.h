#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include "AlertsManagerAPI.h"
#include "AlertsManager.h"

class CAlertsManager;
class CAlertOutput;

class CAlertsSystemClient : public CAlertsManagerAPI {

public:
	CAlertsSystemClient (); 
        CAlertsSystemClient (NotifyTargetTaskIF *m_task);

	~CAlertsSystemClient ();
	NotifyTargetTaskIF * getTask () const
	{
		return m_alert_client_task;
	}
	bool registerSource (std::string src,std::function <void(std::string)> cbk);
        bool unregisterSource (std::string);
	bool addAlert(CAlertsMessage &alertMessage);
	bool deleteAlert (std::string &alert_id);
	void reportAlertID (CAlertsMessage &);
	void notifyActiveAlert (std::string alert_id);
	void notifyObserver(alert_state );
	void registerObserver (CAlertOutput *obs)
	{
		alertObservers.push_back (obs);
	}

	void alertActiveCbk (std::string alert_id);
	bool saveAlertonNV(CAlertsMessage &);
	bool restoreAlertsonPowerUp ();
private:
	bool addAlertAction (CAlertsMessage alertMessage);
	bool deleteAlertAction (std::string alert_id);
	void reportAlertIDAction (CAlertsMessage &);
	bool eraseSavedAlert (std::string &);
	bool isFile (std::string);
	bool isSourceRegistered(std::string);

	CAlertsManager* m_alert_manager;
	NotifyTargetTaskIF *m_alert_client_task;
	std::vector <CAlertOutput *> alertObservers;
	//holds all alert messages from clients
	std::vector <CAlertsMessage > alertMessages;
	std::function <void (std::string)> clientCbk;
        std::map <std::string,std::function <void(std::string)>> source;
};
