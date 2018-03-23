/////////////////////////////////////////////////////////////////////////////////
//@file    DataCollectionClientInterface.cpp
//@brief   Interface between Eddie and DataCollection
///////////////////////////////////////////////////////////////////////////////

///****************************************************************//
//This Code IS TEMPORARY, Will be moving into DataCollectionService//
//*****************************************************************//

#include "DataCollectionClientInterface.h"
#include "DataCollectionClientFactory.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "DeviceManager.pb.h"
#include "AudioBassLevel.pb.h"
#include "WebInterface/Balance.pb.h"
#include "Base64Encoder.h"
#include "DPrint.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"
#include "Balance.pb.h"
#include "Bass.pb.h"
#include "SystemState.pb.h"
#include "SharedProto.pb.h"

static DPrint s_logger( "DataCollectionClientInterface" );


DataCollectionClientInterface::DataCollectionClientInterface( const std::shared_ptr<FrontDoorClientIF> &frontDoorClientIF ):
    m_dataCollectionClientInterfaceTask( IL::CreateTask( "DataCollectionClientInterfaceTask" ) ),
    m_frontDoorClientIF( frontDoorClientIF )
{
    BOSE_DEBUG( s_logger, "DataCollectionClientInterface" );
    Subscribe();
    m_dataCollectionClient =  DataCollectionClientFactory::CreateUDCService();
}

void DataCollectionClientInterface::Subscribe()
{

    AsyncCallback<DeviceManagerPb::DeviceState> DataCollectionStateCb( std::bind( &DataCollectionClientInterface::ProcessSystemState,
                                                                                  this, std::placeholders::_1 ), m_dataCollectionClientInterfaceTask );

    AsyncCallback<ProductPb::AudioBassLevel> DataCollectionbassCb( std::bind( &DataCollectionClientInterface::ProcessBassState,
                                                                              this, std::placeholders::_1 ) , m_dataCollectionClientInterfaceTask );

    AsyncCallback<WebInterface::balance> DataCollectionBalanceCb( std::bind( &DataCollectionClientInterface::ProcessBalanceState,
                                                                             this, std::placeholders::_1 ) , m_dataCollectionClientInterfaceTask );

    //AsyncCallback<DeviceManagerPb::ProductSettings> DataCollectionLanguageCb( std::bind( &DataCollectionClientInterface::ProcessLanguage,
    //                                                                          this, std::placeholders::_1 ) , m_dataCollectionClientInterfaceTask );

    m_frontDoorClientIF->RegisterNotification<DeviceManagerPb::DeviceState>( FRONTDOOR_SYSTEM_STATE_API, DataCollectionStateCb );

    m_frontDoorClientIF->RegisterNotification<ProductPb::AudioBassLevel>( "/audio/bass", DataCollectionbassCb );

    m_frontDoorClientIF->RegisterNotification<WebInterface::balance>( "/audio/balance", DataCollectionBalanceCb );

    //m_frontDoorClientIF->RegisterNotification<DeviceManagerPb::ProductSettings>( "/system/productSettings", DataCollectionLanguageCb );
}

void DataCollectionClientInterface::HandleNowPlayingRequest( const SoundTouchInterface::NowPlaying& nPb, const DeviceManagerPb::DeviceState& ds )
{
    BOSE_DEBUG( s_logger, "System State Process" );
    auto dsPb = std::make_shared<DataCollection::SystemState>();
    SoundTouchInterface::NowPlaying* pnowPlaying = dsPb->mutable_nowplaying();
    *pnowPlaying = nPb;
    std::string j = ProtoToMarkup::ToXML( nPb.container().contentitem(), false );
    std::string contentItemValue = Base64Encoder::Encode( j );
    dsPb->set_contentitem( contentItemValue );
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

void DataCollectionClientInterface::ProcessBassState( const ProductPb::AudioBassLevel& adl )
{
    BOSE_DEBUG( s_logger, "Bass Changed Process" );
    auto dbPb = std::make_shared<DataCollection::Bass>();
    dbPb->set_bass( adl.value() );
    m_dataCollectionClient->SendData( dbPb , "bass-changed" );
}

void DataCollectionClientInterface::ProcessBalanceState( const WebInterface::balance& b )
{
    BOSE_DEBUG( s_logger, "Balance Changed Process" );
    auto dbalPb = std::make_shared<DataCollection::Balance>();
    dbalPb->set_balance( b.targetbalance().text() );
    m_dataCollectionClient->SendData( dbalPb, "balance-changed" );
}
#if 0 //Will remove once Language protobuf is defined in DataCollection
void DataCollectionClientInterface::ProcessLanguage( const DeviceManagerPb::ProductSettings& psPb )
{
    BOSE_DEBUG( s_logger, "Language Changed Process" );
    auto dlPb = std::make_shared<DataCollection::Language>();
    dlPb->set_language( b.language() );
    m_dataCollectionClient->SendData( dlPb, "language-changed" );
}
#endif
