#include <fstream>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "AlertsSystemClient.h"
#include "AlertOutput.h"
#include "playAudio.h"

CAlertsSystemClient :: CAlertsSystemClient()
{
    m_alert_manager = new CAlertsManager();
}

CAlertsSystemClient :: CAlertsSystemClient( NotifyTargetTaskIF *m_task )
    : m_alert_client_task( m_task )
{
    m_alert_manager = new CAlertsManager( this );

    //PJ - <TBD> restore scheduled alerts on power cycle
    restoreAlertsonPowerUp();
}


CAlertsSystemClient :: ~CAlertsSystemClient()
{
}

void CAlertsSystemClient :: alertActiveCbk( std::string alert_id )
{
    std::cout << "System Client callback called!!" << std::endl;
    //notify observers that event has taken place. Need to put
    //smartness in function to ensure only source that requested
    //alert should be notified and not all observers.
    //notifyObserver();

}

//register alert source with callback function.
bool CAlertsSystemClient :: registerSource( std::string src, std::function <void( std::string )> source_cbk )
{
    //search in map for existing src, if exists, return failure/exist messag
    //check for existing alerts which are disabled for the same source, if
    //alerts are disabled, enabled them or report their expiry
    if( source.find( src ) != source.end() )
    {
        std::cout << "source " << src << "already register !!" << std::endl;
        return false;
    }

    //add a source in map with callback function
    source.insert( std::pair <std::string, std::function <void( std::string )>> ( src, source_cbk ) );

    //PJ - <TBD> check if source exists previous, if it does then enable alerts if they are in future and send alert ids to source, if they are in past, delete them, repor it to source

    return true;
}

//unregister source, this can be to ensure alert returns to existing source
bool CAlertsSystemClient :: unregisterSource( std::string src )
{
    //remove source from map, when this is done, disable alerts
    std::map <std::string, std::function <void( std::string )>>::iterator it;

    it = source.find( src );

    if( it != source.end() )
    {
        source.erase( src );
        return false;
    }
    else
        return true;

}

//add alert as per scheduled time. This function registers action callback
//function and returns.
bool CAlertsSystemClient :: addAlert( CAlertsMessage &alertMessage )

{
    //alertMessage.cbk = cbk;
    auto func = std::bind( &CAlertsSystemClient:: addAlertAction, this, alertMessage );
    IL::BreakThread( func, m_alert_manager->getTask() );
    return true;
}

//action callback for addAlert function
bool CAlertsSystemClient :: addAlertAction( CAlertsMessage alertMessage )
{

    auto func = std::bind( &CAlertsSystemClient::alertActiveCbk, this, std::placeholders::_1 );
    ( void ) func;          // TODO: unused
    if( m_alert_manager->addAlert( alertMessage ) )
    {
        std::cout << "alert added successfully" << std::endl;
        //save message in queue only when valid alert id is
        //received from alert manager.
        return true;
    }
    else
    {
        std::cout << "addalert failed!!" << std::endl;
        return false;
    }
}

//if alert is added successfully, alert manager reports alert_id for
//scheduled alert. If alert is successfully added, the alert id should
//not be empty.
void CAlertsSystemClient :: reportAlertID( CAlertsMessage &alertsMessage )
{
    auto func = std::bind( &CAlertsSystemClient::reportAlertIDAction, this, alertsMessage );

    IL::BreakThread( func, m_alert_client_task );
}

//reportAlertID action function
void CAlertsSystemClient :: reportAlertIDAction( CAlertsMessage &alertsMessage )
{
    //add Alert message to the vector
    if( alertsMessage.getAlertId().empty() )
    {
        std::cout << "not a valid alert id, something is wrong.." << std::endl;
        return ;
    }

    //set alert state to scheduled
    alertsMessage.setAlertState( SCHEDULED );

    alertMessages.push_back( alertsMessage );
    //PJ - <TBD> need a better way of report scheduled alert's id
    //alertsMessage.cbk (alertsMessage.getAlertId());
    std::cout << "scheduled alert with id - " << alertsMessage.getAlertId() << std::endl;

    //save scheduled alert to persistent memory
    saveAlertonNV( alertsMessage );

    std::map <std::string, std::function <void( std::string )>>::iterator it;
    for( it = source.begin(); it != source.end(); ++it )
    {
        //PJ - <TBD> need to add logic to parse through sources to identify source and call the callback. In addition, need to report the status of alert, along with id to make common reporting callback with source so that source comes to know whats going on with alert when alert changes states
        auto func = source["web"];
        func( alertsMessage.getAlertId() );
        break;
    }


}

//delete alert with alert id
bool CAlertsSystemClient :: deleteAlert( std::string &alert_id )

{
    auto func = std::bind( &CAlertsSystemClient:: deleteAlertAction, this, alert_id );
    IL::BreakThread( func, m_alert_manager->getTask() );
    return true;
}

//
bool CAlertsSystemClient :: deleteAlertAction( std::string alert_id )
{
    m_alert_manager->deleteAlert( alert_id );
    std::vector <CAlertsMessage >::iterator it;
    int msg_idx = 0;
    bool alertDeleted = false;

    for( it = alertMessages.begin(); it < alertMessages.end(); it++ )
    {
        //std::cout <<"id - " << it->getAlertId() << std::endl;
        if( alert_id == it->getAlertId() )
        {
            //check if alert is active notify to observer
            //std::cout << "alert state = " << it->getAlertState()<< std::endl;
            if( it->getAlertState() == ACTIVE )
            {
                std::cout << "deleeting active alert ..." << std::endl;
                it->setAlertState( ACKNOWLEDGED );
                //notify observer that alert is acknowledged
                notifyObserver( it->getAlertState() );
            }
            alertMessages.erase( alertMessages.begin() + msg_idx );
            alertDeleted = true;
            std::cout << "message deleted " << std::endl;
            //remove alert file
            eraseSavedAlert( alert_id );
            break;
        }

        msg_idx++;
    }
    return alertDeleted;
}

void CAlertsSystemClient :: notifyActiveAlert( std::string alert_id )
{
    //notify alert to observer
    std::cout << "Alert active - " << alert_id << std::endl;
    //notify observers reg. active alert
    //notifyObserver (alert_id);

    std::vector <CAlertsMessage >::iterator it;

    for( it = alertMessages.begin(); it < alertMessages.end(); it++ )
    {
        if( alert_id == it->getAlertId() )
        {
            //set alert state to scheduled
            it->setAlertState( ACTIVE );

            notifyObserver( it->getAlertState() );
            break;
        }
    }

}

void CAlertsSystemClient :: notifyObserver( alert_state st )
{
    //PJ - <TBD> add logic to send notification to only source which
    //requested for the alert instead of all observers.

    for( unsigned int i = 0; i < alertObservers.size(); i++ )
    {
        if( st == ACTIVE )
            alertObservers[i]->onActiveAlert();
        else if( st == ACKNOWLEDGED )
            alertObservers[i]->onAlertAcknowledged();
    }
}

bool CAlertsSystemClient :: saveAlertonNV( CAlertsMessage &alertMessage )
{
    //need to replace file with a location, from a config file
    //where all alerts files can be stored
    //PJ - <TBD> instead of alert id scheduled time should be used
    // as a filename or combination of both to ensure uniquness as well
    //as uniquely identifying alert at boot

    std::string alert_file = "./Alerts/" + alertMessage.getAlertId();

    //PJ - <TBD> need to decide format.
    std::string strtowrite = alertMessage.MessagetoCSV();
    std::ofstream outAlertStrm( alert_file, std::ios::binary );
    //outAlertStrm.open();
    if( !outAlertStrm.is_open() )
        return false;
    outAlertStrm << strtowrite;
    std::cout << "message string - " << strtowrite.c_str() << std::endl;
    outAlertStrm.close();
    return true;
}

bool CAlertsSystemClient :: restoreAlertsonPowerUp()
{
    DIR *dp;
    struct dirent *dirp;
    std::string baseDir = "./Alerts/";

    if( ( dp = opendir( baseDir.c_str() ) ) == NULL )
    {
        std::cout << "[ERROR: " << errno << " ] Couldn't open " << baseDir << "." << std::endl;
        return false;
    }
    else
    {
        while( ( dirp = readdir( dp ) ) != NULL )
        {
            if( dirp->d_name != std::string( "." ) && dirp->d_name != std::string( ".." ) )
            {
                if( isFile( baseDir + dirp->d_name ) == true )
                {
                    std::cout << "[FILE]\t" << baseDir << dirp->d_name << "/" << std::endl;
                    //PJ - <TBD> parse message and schedule alert
                    std::ifstream istrm( baseDir + static_cast <std::string>( dirp->d_name ) );
                    if( !istrm.is_open() )
                        continue;
                    std::string tmp;
                    istrm >> tmp;
                    CAlertsMessage newMessage;
                    newMessage.CSVtoMessage( tmp );
                    //PJ - <TBD> if the scheduled time is in past, do not schedule alert, delete alert file, may be notify alert to source and continue
                    addAlert( newMessage );
                }
            }
        }
        closedir( dp );
    }
    return true;
}

bool CAlertsSystemClient ::isFile( std::string dir )
{
    struct stat fileInfo;
    stat( dir.c_str(), &fileInfo );
    if( S_ISREG( fileInfo.st_mode ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CAlertsSystemClient :: eraseSavedAlert( std::string &alert_id )
{
    if( alert_id.empty() )
        return false;

    std::string fname = "./Alerts/" + alert_id;
    const char *tmp = fname.c_str();

    int ret = std::remove( tmp );

    if( !ret )
    {
        std::cout << "alert erased successfully" << std::endl;
        return true;
    }
    else
    {
        std::cout << "alert file not found " << std::endl;
        return false;
    }
}

bool CAlertsSystemClient ::isSourceRegistered( std::string src )
{
    std::map <std::string, std::function <void( std::string )>>::iterator it;

    it = source.find( src );

    //if source doesn't exist return false
    if( it == source.end() )
        return false;

    return true;
}

