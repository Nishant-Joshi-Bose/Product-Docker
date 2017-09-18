#pragma once
#include "AlertOutput.h"
#include "APTaskFactory.h"
//#include "BreakThread.h"

class CAlsaAlertOutput : public CAlertOutput
{
public:
    CAlsaAlertOutput( CAlertsSystemClient *aClient, NotifyTargetTaskIF *m_task ):
        CAlertOutput( aClient ),
        m_alsa_task( m_task ),
        m_worker_task( IL::CreateTask( "workerPlayback" ) )
    {
    }
    void onActiveAlert();
    void onAlertAcknowledged() ;
private:
    void onActiveAlertAction();
    void onAlertAcknowledgedAction();
    NotifyTargetTaskIF *m_alsa_task;
    NotifyTargetTaskIF *m_worker_task;
    std:: queue <std::string> active;
};
