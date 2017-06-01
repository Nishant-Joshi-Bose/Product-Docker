#pragma once

#include <iostream>
#include <functional>

typedef enum {
	CREATED = 0, 
	SCHEDULED ,
	ACTIVE,
	ACKNOWLEDGED,
	DISABLED	//when source has requested alert and later got disconnected
}alert_state; 

class CAlertsMessage {
public:
	CAlertsMessage () {};
	CAlertsMessage (std::string schedule_time, std::string a_type) :
		sch_time (schedule_time),
		alert_type (a_type),
		state (CREATED)
	{
	}
	~CAlertsMessage () {};
	std::string getScheduledTime ();
	std::string getAlertType ();
	std::string getAlertId ();
	std::string getAlertSource ();
	alert_state getAlertState ();
	void setScheduledTime (std::string );
	void setAlertType (std::string);
	void setAlertId (std::string);
	void setAlertSource (std::string);
	void setAlertState (alert_state);
	std::string MessagetoCSV ();
	bool CSVtoMessage (std::string );

#if 0
	CAlertsMessage& operator= (const CAlertsMessage &rhs)
	{
		CAlertsMessage tmp;
		tmp.sch_time = rhs.sch_time;
		*this = tmp;
		return *this; 		
	}
#endif
	std::function <void(std::string)> cbk;
private:
	std::string MessagetoJSON ();
	CAlertsMessage JSONtoMessage ();
	std::string sch_time;
	std::string alert_id;
	std::string alert_type;
	std::string alert_source;
	alert_state state;
};
