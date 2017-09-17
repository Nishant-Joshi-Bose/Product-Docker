#include <iostream>
#include <time.h>
#include "AlertsScheduler.h"


//check if alert exists for the specified time, if not add alert, add alert in
//vector
std::string CAlertsScheduler :: addAlert( struct tm* t_time, std::function <void ( std::string )> cbk )
{
    if( checkAlertExist( t_time ) )
        return "";

    CAlerts* new_Alert = new CAlerts( m_ap_task );

    std::string alert_id = new_Alert->addAlert( t_time, cbk );
    if( !alert_id.empty() )
        m_alerts.push_back( *new_Alert );
    return alert_id;
}

//deletes alert based on alert_id.
bool CAlertsScheduler :: deleteAlert( std::string alert_message_id )
{
    bool alertDeleted = false;
    int vector_index = 0;
    for( m_alerts_it = m_alerts.begin(); m_alerts_it < m_alerts.end(); m_alerts_it++ )
    {
        CAlerts tmp_m_alert = *m_alerts_it;
        if( tmp_m_alert.getAlertId() == alert_message_id )
        {
            tmp_m_alert.deleteAlert();
            m_alerts.erase( m_alerts.begin() + vector_index );
            std::cout << "alert deleted - " << alert_message_id << std::endl;
            alertDeleted = true;
            break;
        }
        vector_index++;
    }
    return alertDeleted;
}

//check if the alert exists based on scheduled time, if alert exists
//do not add alert. The subsystem supports multiple timers for the same
//time so if needed check can be removed
bool CAlertsScheduler :: checkAlertExist( struct tm *t_time )
{
    bool result = false;
    std::vector <CAlerts>::iterator it;

    //check for existing alert for same tm.
    for( it = m_alerts.begin(); it < m_alerts.end(); )
    {
        if( difftime( it->getScheduledUTCTime(), timegm( t_time ) ) == 0 )
        {
            result = true;
            break;
        }
        if( it == m_alerts.end() )
            break;
        else
            it++;
    }

    return result;
}
