#include <iostream>
#include <iomanip>
#include <cstdio>
#include "AlertsManager.h"


CAlertsManager :: CAlertsManager() :
    m_pTask( IL::CreateTask( "AlertsManager" ) ),
    m_alertsScheduler( m_pTask ),
    scheduledAlerts( 0 ),
    activeAlerts( 0 )
{
    m_plogger = new DPrint( "AlertsManager" );
}

CAlertsManager :: CAlertsManager( CAlertsSystemClient *m_client ) :
    m_pTask( IL::CreateTask( "AlertsManager" ) ),
    m_alertsScheduler( m_pTask ),
    m_alert_client( m_client ),
    scheduledAlerts( 0 ),
    activeAlerts( 0 )
{
    m_plogger = new DPrint( "AlertsManager" );
}

CAlertsManager :: ~CAlertsManager()
{
    delete m_plogger;
}

bool CAlertsManager :: addAlert( CAlertsMessage &alertMessage )
{
    //per requirements max scheduled alerts should not be more than 100
    //PJ - <TBD> we may want to move this to config file from here.
    std::cout << "***current scheduled alert cout - " << scheduledAlerts << std::endl;
    if( scheduledAlerts >= MAX_SCHEDULED_ALERTS )
    {
        std::cout << "&&&max alert count reached " << std::endl;
        return false;
    }

    auto func = std::bind( &CAlertsManager::addAlertAction, this, alertMessage );
    IL::BreakThread( func, m_pTask );
    return true;
}

bool CAlertsManager :: addAlertAction( CAlertsMessage &alertMessage )
{
    std::string alert_time = alertMessage.getScheduledTime();
    std::tm tmb = get_Time( alert_time );
//        std::cout << std::put_time (&tmb,"%FT%T");
    //PJ - commented to replace callback from client with local callback
    //alertMessage.setAlertId (m_alertsScheduler.addAlert(&tmb, cbk));
    auto mgr_cbk = std::bind( &CAlertsManager::notifyActiveAlert, this, std::placeholders::_1 );
    alertMessage.setAlertId( m_alertsScheduler.addAlert( &tmb, mgr_cbk ) );

    reportAlertID( alertMessage );
    return true;
}

std::tm CAlertsManager :: get_Time( std::string &alert_time )
{
    int yr, mon;//,day,hr,min,sec;
    std::tm tmb;

    sscanf( alert_time.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", &yr, &mon, &tmb.tm_mday, &tmb.tm_hour, &tmb.tm_min, &tmb.tm_sec );

    tmb.tm_year = yr - 1900;
    tmb.tm_mon = mon - 1;
    return tmb;
}

bool CAlertsManager :: deleteAlert( std::string &alert_id )
{
    //std::cout <<"delete alert with alert id "<<alert_id<<std::endl;
    m_plogger->LogInfo( "delete alert with alert id %s", alert_id.c_str() );

    //need to replace message id with alert_id. The alert_id is the way
    //alert manager should figure out the alert as it is generated by
    //alert manager. Once logic to generate alert id is in place
    //alert_id will be returned to the source requesting to set
    //alert/alarm. When it wants to remove it, source needs to provide
    //alert_id.
    if( m_alertsScheduler.deleteAlert( alert_id ) )
        //update scheduled alert after deletion
        scheduledAlerts--;
    return true;
}

void CAlertsManager  :: reportAlertID( CAlertsMessage &alertMessage )
{
    auto func = std::bind( &CAlertsManager::reportAlertIDAction, this, alertMessage );

    IL::BreakThread( func, m_alert_client->getTask() );
}


void CAlertsManager  :: reportAlertIDAction( CAlertsMessage alertMessage )
{
    std::cout << "alert manager - alert id " << alertMessage.getAlertId() << std::endl;
    m_alert_client->reportAlertID( alertMessage );
    //update scheduled alert count if alert is added successfully
    if( !alertMessage.getAlertId().empty() )
        scheduledAlerts++;
    std::cout << "current scheduled alert cout - " << scheduledAlerts << std::endl;
}

void CAlertsManager :: notifyActiveAlert( std::string alert_id )
{
    auto func = std::bind( &CAlertsManager::notifyActiveAlertAction, this, alert_id );

    IL::BreakThread( func, m_alert_client->getTask() );

}

void CAlertsManager :: notifyActiveAlertAction( std::string alert_id )
{
    std::cout << "alert manager - notify alert " << alert_id << std::endl;
    //notify client that alert is active
    m_alert_client->notifyActiveAlert( alert_id );
}
