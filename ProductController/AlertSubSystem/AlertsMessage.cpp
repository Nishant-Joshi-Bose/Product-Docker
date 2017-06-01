#include <cstring>

#include "AlertsMessage.h"


std::string CAlertsMessage :: getScheduledTime ()
{
	return sch_time;
}

std::string CAlertsMessage :: getAlertType ()
{
	return alert_type;
}


std::string CAlertsMessage :: getAlertId ()
{
	return alert_id;
}

std::string CAlertsMessage :: getAlertSource ()
{
	return alert_source;
}

alert_state CAlertsMessage :: getAlertState ()
{
	return state;
}

void CAlertsMessage :: setScheduledTime (std::string schedule_time)
{
	sch_time = schedule_time;
}

void CAlertsMessage :: setAlertType (std::string a_type)
{
	alert_type = a_type;
}


void CAlertsMessage :: setAlertId (std::string a_id)
{
	alert_id = a_id;
}

void CAlertsMessage :: setAlertSource (std::string a_source)
{
	alert_source = a_source;
}

void CAlertsMessage :: setAlertState (alert_state st)
{
	state = st;
}

std::string CAlertsMessage:: MessagetoJSON ()
{
    return std::string("");
}

std::string CAlertsMessage:: MessagetoCSV ()
{
	std::string str ;
	str = getScheduledTime () + ","+
		getAlertType () + "," + 
		getAlertSource () + ","+ 
		getAlertId () + "\n";

	return str;
}

bool CAlertsMessage:: CSVtoMessage (std::string message)
{
	//PJ - very un cpp way of doing...
	char tmp[100]; 
	strcpy(tmp,message.c_str());

	//char *tok = strtok (message,",");
	char *tok = strtok (tmp,",");
	if (tok != nullptr)
	{
		sch_time = static_cast <std::string> (tok);
		tok = strtok(NULL,",");
		if (tok != nullptr)
		{
			alert_type = static_cast <std::string> (tok);
			tok = strtok (NULL,",");
			if (tok != nullptr)
				alert_source = static_cast<std::string> (tok);
		}

	}
	return true;
}

CAlertsMessage CAlertsMessage:: JSONtoMessage ()
{
return CAlertsMessage();
}
