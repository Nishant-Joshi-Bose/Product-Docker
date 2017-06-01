#include <thread> //need to replace with aptask but don't know how
#include "AlsaAlertOutput.h"
#include "playAudio.h"
#include "BreakThread.h"

static bool alert_in_progress=false;
char alarm_file[100] = "./chime.wav";


void CAlsaAlertOutput :: onActiveAlert ()
{
	//std::cout << "Alert has taken place, play audio" << std::endl;

	auto func = std::bind (&CAlsaAlertOutput::onActiveAlertAction, this);

	IL::BreakThread (func,m_worker_task);
}

void CAlsaAlertOutput :: onActiveAlertAction ()
{
	active.push("src_name_goes_here");

	std::cout << "active queue size = " << active.size() << std::endl;
	if (!alert_in_progress) { 
		auto func = std::bind (&playAlert,alarm_file);

		std::thread th_play (func);

		th_play.detach(); 
		alert_in_progress = true;
	}
}

void CAlsaAlertOutput :: onAlertAcknowledged ()
{
	//on button event or receiving voice command, stop playingback
	// audio alert tone
	std::cout <<"stopping audio playback..." << std::endl;	
	//auto func = std::bind (&CAlsaAlertOutput::onAlertAcknowledgedAction, this);
	//IL::BreakThread (func, m_worker_task);
	if (!active.empty())
		active.pop();
	if (active.empty())
	{
		stopAlert();
		alert_in_progress = false;
	}
        	
}

void CAlsaAlertOutput :: onAlertAcknowledgedAction ()
{
	stopAlert();
}
