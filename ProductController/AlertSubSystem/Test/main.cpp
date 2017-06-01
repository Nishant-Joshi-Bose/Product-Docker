#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include "UnitTest++/UnitTest++.h"
#include "Alerts.h"
#include "AsyncCallback.h"
#include "APTaskFactory.h"
#include "AlertsScheduler.h"
#include "AlertsMessage.h"
#include "AlertsSystemClient.h"

static std::string galert_id;

void test_cbk (std::string aid)
{
	std::cout <<"**"<< aid << std::endl;
}
#if 1 
SUITE (TestAlerts)
{
TEST (AddAlertInvalidTimeInFuture)
{
	NotifyTargetTaskIF *m_test = IL :: CreateTask ("test");
	//CAlerts testalert(IL::CreateTask("test"));
	CAlerts testalert(m_test);
	time_t rawtime = time(nullptr);
	//adding the limitation is that alarm cannot be set for 
	//more than 24 hours. Setting alarm for 24 hour and 10 second
	//should fail the alarm 
	rawtime = rawtime + (24 * 60 * 60) + 10;
	struct tm *t_tm = gmtime(&rawtime);
	std::string aid = testalert.addAlert(t_tm, &test_cbk); 
	CHECK (aid.empty()); 
}
TEST (AddAlertInvalidTimeInPast)
{

	NotifyTargetTaskIF *m_test = IL :: CreateTask ("test");
	CAlerts testalert(m_test);
	time_t rawtime = time(nullptr);
	//adding the limitation is that alarm cannot be set for 
	//more than 24 hours. Setting alarm for 24 hour and 10 second
	//should fail the alarm 
	rawtime = rawtime - 10;
	struct tm *t_tm = gmtime(&rawtime);
	std::string aid = testalert.addAlert(t_tm, &test_cbk); 
	CHECK (aid.empty()); 

}
TEST (AddAlertNullCallback)
{

	NotifyTargetTaskIF *m_test = IL :: CreateTask ("test");
	CAlerts testalert(m_test);
	time_t rawtime = time(nullptr);
	//valid time in future 
	rawtime = rawtime + 10;
	struct tm *t_tm = gmtime(&rawtime);
	std::string aid = testalert.addAlert(t_tm, nullptr); 
	CHECK (aid.empty()); 

}
TEST (DeleteAlert)
{
	NotifyTargetTaskIF *m_test = IL :: CreateTask ("test");
	CAlerts testalert(m_test);
	time_t rawtime = time(nullptr);
	//valid time in future 
	rawtime = rawtime + 10;
	struct tm *t_tm = gmtime(&rawtime);
	std::string aid = testalert.addAlert(t_tm, &test_cbk); 
	sleep (2); //sleep for 2 seconds before deleting alert
	bool result = testalert.deleteAlert ();
	CHECK (result);
}


TEST (GetAlertId)
{
	NotifyTargetTaskIF *m_test = IL :: CreateTask ("test");
	CAlerts testalert(m_test);
	time_t rawtime = time(nullptr);
	//valid time in future 
	rawtime = rawtime + 10;
	struct tm *t_tm = gmtime(&rawtime);
	std::string aid = testalert.addAlert(t_tm, &test_cbk); 
	sleep (1); //sleep for 2 seconds before checking alert id
	std::string alert_id = testalert.getAlertId ();
	CHECK (!alert_id.empty());
        bool result = testalert.deleteAlert ();

}
}
SUITE (TestAlertScheduler)
{
	TEST (SchedulerAddAlert)
	{
		CAlertsScheduler asch (IL::CreateTask("test"));
		time_t rawtime = time(nullptr);
		//valid time in future
		rawtime = rawtime + 10;
		struct tm *t_tm = gmtime(&rawtime);
		std::string a_id = asch.addAlert (t_tm, &test_cbk);	
		CHECK (!a_id.empty());	
		sleep (1);
		bool result = asch.deleteAlert (a_id);
	}


	TEST (SchedulerAddAlertSameTime)
	{
		CAlertsScheduler asch (IL::CreateTask("test"));
		time_t rawtime = time(nullptr);
		//valid time in future
		rawtime = rawtime + 10;
		struct tm *t_tm = gmtime(&rawtime);
		std::string a_id = asch.addAlert (t_tm, &test_cbk);	
		sleep (2);
                rawtime = rawtime + 10;
                t_tm = gmtime(&rawtime);
                a_id = asch.addAlert (t_tm, &test_cbk);
		//adding alert for the same time. The test should fail
		a_id = asch.addAlert (t_tm, &test_cbk);	
		CHECK (a_id.empty());	
		//for next test to start with a different time then current test
		sleep (1);
	}

	TEST (SchedulerDeleteAlert)
	{
		CAlertsScheduler asch (IL::CreateTask("test"));
		time_t rawtime = time(nullptr);
		//valid time in future
		rawtime = rawtime + 10;
		struct tm *t_tm = gmtime(&rawtime);
		std::string a_id = asch.addAlert (t_tm, &test_cbk);	
		sleep (2);
                rawtime = rawtime + 10;
                t_tm = gmtime(&rawtime);
                std::string sec_a_id = asch.addAlert (t_tm, &test_cbk);
		//adding alert for the same time. The test should fail
		bool result = asch.deleteAlert (a_id);	
		CHECK (result);	
		result = asch.deleteAlert(sec_a_id);
	}
}
#endif
#if 1
SUITE (TestAlertsManager)
{
	TEST (TestAlertManagerAddAlert)
	{
		//create client, client will create Alert Manager
		CAlertsSystemClient* m_pAlerts_client = new CAlertsSystemClient (IL::CreateTask("AlertsSystemClient"));
		CAlertsMessage testmessage;
		std::ostringstream oss;
		struct tm *t_time;

		time_t rawtime = time (nullptr);
		rawtime += 10;
		t_time = gmtime (&rawtime);
		oss << std::put_time(t_time,"%FT%T");
		std::cout << "time = " << oss.str() << std::endl;
		testmessage.setScheduledTime (oss.str());
		testmessage.setAlertType ("TIMER");
		auto func = std::bind ([] (std::string a_id) {galert_id =a_id;std::cout<<"***" << a_id << std::endl;},std::placeholders::_1);
		//m_pAlerts_client -> addAlert (testmessage, test_cbk);
		m_pAlerts_client -> addAlert (testmessage);
		sleep(2);
		CHECK (!galert_id.empty());
		m_pAlerts_client -> deleteAlert (galert_id);
		delete m_pAlerts_client;
		galert_id = "";
		sleep(10);
	}

	TEST (TestAlertManagerDeleteAlert)
	{
		//create client, client will create Alert Manager
		CAlertsSystemClient* m_pAlerts_client = new CAlertsSystemClient (IL::CreateTask("AlertsSystemClient"));
		CAlertsMessage testmessage;
		std::ostringstream oss;
		struct tm *t_time;

		time_t rawtime = time (nullptr);
		rawtime += 10;
		t_time = gmtime (&rawtime);
		oss << std::put_time(t_time,"%FT%T");
		std::cout << "time = " << oss.str() << std::endl;
		testmessage.setScheduledTime (oss.str());
		testmessage.setAlertType ("TIMER");
		auto func = std::bind ([] (std::string a_id) {galert_id = a_id;std::cout<<"***" << a_id << std::endl;},std::placeholders::_1);
		//m_pAlerts_client -> addAlert (testmessage);
		m_pAlerts_client -> addAlert (testmessage);
		sleep(2);
		bool result = m_pAlerts_client -> deleteAlert (galert_id);
		CHECK (result);
		sleep(1);
		delete m_pAlerts_client;
		galert_id = "";
	}
	TEST (TestMaxAlert)
	{
		std::cout << "max alert test started ..." << std::endl;
		//create client, client will create Alert Manager
		CAlertsSystemClient* m_pAlerts_client = new CAlertsSystemClient (IL::CreateTask("AlertsSystemClient"));
		CAlertsMessage testmessage;
		struct tm *t_time;

		auto func = std::bind ([] (std::string a_id) {galert_id =a_id;std::cout<<"***" << a_id << std::endl;},std::placeholders::_1);
		time_t rawtime = time (nullptr);
		for (int indx = 0; indx < 5;indx++)
		{
			std::ostringstream oss;
			rawtime += 10;
			t_time = gmtime (&rawtime);
			oss << std::put_time(t_time,"%FT%T");
			std::cout << "time = " << oss.str() << std::endl;
			testmessage.setScheduledTime (oss.str());
			testmessage.setAlertType ("TIMER");
			m_pAlerts_client -> addAlert (testmessage);
			sleep(1);
		}
		sleep(2);
		galert_id = "";
		std::ostringstream oss;
	        rawtime += 10;
		t_time = gmtime (&rawtime);
		oss << std::put_time(t_time,"%FT%T");
		std::cout << "time = " << oss.str() << std::endl;
		testmessage.setScheduledTime (oss.str());
		testmessage.setAlertType ("TIMER");
		m_pAlerts_client -> addAlert (testmessage);
		//wait for all the alerts to get scheduled
		sleep(10);

		CHECK (galert_id.empty());
		delete m_pAlerts_client;
		galert_id = "";
		sleep(10);
	}

}
#endif

SUITE (TestAlertsSystemClient)
{

	TEST(ClientAddAlert)
	{


	}
	
	TEST(ClientDeleteAlert)
	{


	}
	
	TEST(ClientPowerupRestore)
	{


	}

}

int main ()
{
	return UnitTest::RunAllTests();
}
