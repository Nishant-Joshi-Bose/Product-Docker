/////////////////////////////////////////////////////////////////////////////////
//@file    DataCollectionClientInterface.cpp
//@brief   Interface between Eddie and DataCollection
///////////////////////////////////////////////////////////////////////////////


#include "DataCollectionClientInterface.h"
#include "DataCollectionClientFactory.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "DeviceManager.pb.h"
#include "DPrint.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "SystemState.pb.h"

static DPrint s_logger( "DataCollectionClientInterface" );


DataCollectionClientInterface::DataCollectionClientInterface( const std::shared_ptr<FrontDoorClientIF> &frontDoorClientIF, const std::shared_ptr<DataCollectionClientIF>& dataCollectionPTR ):
    m_dataCollectionClientInterfaceTask( IL::CreateTask( "DataCollectionClientInterfaceTask" ) ),
    m_frontDoorClientIF( frontDoorClientIF ),
    m_dataCollectionClient( dataCollectionPTR )
{
    BOSE_DEBUG( s_logger, "DataCollectionClientInterface" );
    Subscribe();
}

void DataCollectionClientInterface::Subscribe()
{

    AsyncCallback<DeviceManagerPb::DeviceState> DataCollectionStateCb( std::bind( &DataCollectionClientInterface::ProcessSystemState,
                                                                                  this, std::placeholders::_1 ), m_dataCollectionClientInterfaceTask );


    m_frontDoorClientIF->RegisterNotification<DeviceManagerPb::DeviceState>( FRONTDOOR_SYSTEM_STATE_API, DataCollectionStateCb );

}

void DataCollectionClientInterface::HandleNowPlayingRequest( const SoundTouchInterface::NowPlaying& nPb, const DeviceManagerPb::DeviceState& ds )
{
    BOSE_DEBUG( s_logger, "System State Process" );
    auto dsPb = std::make_shared<DataCollection::SystemState>();
    SoundTouchInterface::NowPlaying* pnowPlaying = dsPb->mutable_nowplaying();
    *pnowPlaying = nPb;
    dsPb->set_systemstate( ds.state() );
    m_dataCollectionClient->SendData( dsPb , "system-state-changed" );
}

void DataCollectionClientInterface::GetCallbackError( const FrontDoor::Error& error )
{
    BOSE_WARNING( s_logger, "%s: Error = (%d-%d) %s", __func__, error.code(), error.subcode(), error.message().c_str() );
}

void DataCollectionClientInterface::ProcessSystemState( const DeviceManagerPb::DeviceState& ds )
{
    auto func = [this, ds]( const SoundTouchInterface::NowPlaying & noPb )
    {
        HandleNowPlayingRequest( noPb, ds );
    };
    auto errorfunc = [this]( const FrontDoor::Error & error )
    {
        GetCallbackError( error );
    };
    AsyncCallback<SoundTouchInterface::NowPlaying> getNowPlayingReqCb( func, m_dataCollectionClientInterfaceTask );
    AsyncCallback<FrontDoor::Error> errorCb( errorfunc, m_dataCollectionClientInterfaceTask );
    m_frontDoorClientIF->SendGet<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_NOWPLAYING_API , getNowPlayingReqCb, errorCb );
}
