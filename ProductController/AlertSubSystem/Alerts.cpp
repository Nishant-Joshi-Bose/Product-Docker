#include <iostream>
#include <string>
#include "Alerts.h"

#define MAX_SECONDS_IN_DAY ( 24 * 60 * 60)

CAlerts :: CAlerts( NotifyTargetTaskIF *m_task )
{
    m_alert_task = m_task;
}

CAlerts:: ~CAlerts()
{

}
void CAlerts::callback_cbk()
{
    std::cout << "callback called" << std::endl;
    alert_callback( alert_id );
    //std::cout << "timer uuid " << this->alert_id <<" is called " << std::endl;
}
std::string CAlerts:: addAlert( struct tm *sch_time, std::function <void( std::string )> cbk )
{
    if( cbk != nullptr )
        alert_callback = cbk;
    else
    {
        alert_id = "";
        return alert_id;
    }

    alert_callback = cbk;

    m_alert_timer = APTimerFactory::CreateTimer( m_alert_task, "Timer" + message_id );
    scheduled_time = *sch_time;
    //convert UTC time into local time
    time_t utc_time = timegm( sch_time );
    time_t raw_time = time( nullptr );
    struct tm *rawtime = gmtime( &raw_time );
    raw_time = timegm( rawtime );
    std::cout << "utc time = " << utc_time << std::endl;
    std::cout << "cur time = " << raw_time << std::endl;
//  struct tm* t_time = localtime(&raw_time);

    //time_t scheduled_time = difftime (l_time,time(nullptr));
    time_t sched_time = difftime( utc_time, time( nullptr ) );
    std::cout << "diff time = " << sched_time << std::endl;

    if( sched_time > 0 && sched_time < MAX_SECONDS_IN_DAY )
        m_alert_timer->SetTimeouts( sched_time * 1000, 0 );
    else
    {
        std::cout << "cannot set timer" << std::endl;
        return "" ;
    }
    //add code to store the message id, token, schedule time to  a file or permanent storage area
    auto func_cbk = std::bind( &CAlerts::callback_cbk, this );
    m_alert_timer->Start( func_cbk );
    //uuid for alert id which will be unique
    uuid_t alert_uuid;
    char a_id[37];

    uuid_generate( alert_uuid );
    uuid_unparse_upper( alert_uuid, a_id );
    alert_id = static_cast <std::string>( a_id );
    std::cout << "alert iD = " << alert_id << std::endl;
    return alert_id;
}

std::string CAlerts :: getMessageId()
{
    return message_id;
}
std::string CAlerts :: getAlertId()
{
    return alert_id;
}
bool CAlerts :: deleteAlert()
{
    m_alert_timer ->Stop();
    return true;
}
time_t CAlerts :: getScheduledUTCTime()
{
    return timegm( &scheduled_time );
}

