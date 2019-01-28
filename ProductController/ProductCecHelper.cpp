////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductCecHelper.cpp
///
/// @brief     This header file contains declarations for managing the interface with A4VVideoManager
///
/// @author    Manoranjani Malisetti
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iomanip>
#include "Utilities.h"
#include "CustomProductController.h"
#include "ProductCecHelper.h"
#include "FrontDoorClient.h"
#include "EndPointsDefines.h"
#include "ProductEndpointDefines.h"
#include "PGCErrorCodes.h"
#include "DataCollectionClientFactory.h"
#include "ProductDataCollectionDefines.h"
#include "ProductSTS.pb.h"

using namespace ProductPb;
constexpr char cecModePersistPath[] = "CecMode.json";
namespace
{
const std::string s_ModeOn         = "ON";
const std::string s_ModeOff        = "OFF";
const std::string s_ModeAltOn      = "ALTERNATE_ON";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::ProductCecHelper
///
/// @param  CustomProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCecHelper::ProductCecHelper( CustomProductController& ProductController )

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
      m_connected( false ),
      m_CustomProductController( static_cast< CustomProductController & >( ProductController ) ),
      m_DataCollectionClient( DataCollectionClientFactory::CreateUDCService( m_ProductTask ) ),
      m_FrontDoorClient( ProductController.GetFrontDoorClient( ) )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::Run
///
/// @brief  This method connects and starts the handling of communication with A4VVideoManager
///         service.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductCecHelper::Run( )
{
    BOSE_DEBUG( s_logger, "The hardware connection to the A4VVideoManager is being established." );
    m_cecModePersistence = ProtoPersistenceFactory :: Create( cecModePersistPath, g_ProductPersistenceDir );
    LoadFromPersistence();

    m_CecHelper = A4VVideoManagerClientFactory::Create( "ProductCecHelper", m_ProductTask );
    Callback< bool > ConnectedCallback( std::bind( &ProductCecHelper::Connected,
                                                   this,
                                                   std::placeholders::_1 ) );

    m_CecHelper->Connect( ConnectedCallback );

    ///
    /// Registration as a client for getting notification of changes in the now playing state from
    /// CAPS is made through the FrontDoorClient object pointer. The callback HandleCapsNowPlaying
    /// is used to receive these notifications.
    ///
    AsyncCallback< SoundTouchInterface::NowPlaying >
    callback( std::bind( &ProductCecHelper::HandleNowPlaying,
                         this, std::placeholders::_1 ),
              m_ProductTask );

    m_FrontDoorClient->RegisterNotification< SoundTouchInterface::NowPlaying >(
        FRONTDOOR_CONTENT_NOWPLAYING_API,
        callback );


    auto getFunc = [ this ]( Callback< const CecModeResponse>   resp,
                             Callback<FrontDoor::Error>         errorRsp )
    {
        CecModeHandleGet( resp, errorRsp );
    };

    AsyncCallback<Callback< CecModeResponse >, Callback< FrontDoor::Error > >getCb( getFunc,
                                                                                    m_ProductTask );

    m_GetConnection = m_FrontDoorClient->RegisterGet( FRONTDOOR_CEC_API,
                                                      getCb,
                                                      FrontDoor::PUBLIC,
                                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    auto putFunc = [ this ]( CecUpdateRequest                   cecReq,
                             Callback< const CecModeResponse >  cecResp,
                             Callback< FrontDoor::Error >       errorRsp )
    {
        CecModeHandlePut( cecReq, cecResp, errorRsp );
    };

    AsyncCallback< const CecUpdateRequest,
                   Callback< CecModeResponse >,
                   Callback< FrontDoor::Error > >
                   putCb( putFunc, m_ProductTask );

    m_PutConnection = m_FrontDoorClient->RegisterPut<CecUpdateRequest>(
                          FRONTDOOR_CEC_API,
                          putCb,
                          FrontDoor::PUBLIC,
                          FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                          FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    //System power control notification registration and callback handling
    auto handleSystemPowerControl = [this]( SystemPowerPb::SystemPowerControl systemPowerControlState )
    {
        if( systemPowerControlState.power() == SystemPowerPb::SystemPowerControl_State_ON )
        {
            PowerOn();
        }
        else
        {
            PowerOff();
        }
    };
    AsyncCallback< SystemPowerPb::SystemPowerControl > powerCb( handleSystemPowerControl, m_ProductTask );
    m_FrontDoorClient->RegisterNotification<SystemPowerPb::SystemPowerControl>( FRONTDOOR_SYSTEM_POWER_CONTROL_API, powerCb );
    m_FrontDoorClient->SendGet<SystemPowerPb::SystemPowerControl, FrontDoor::Error>( FRONTDOOR_SYSTEM_POWER_CONTROL_API, powerCb, {} );

    //Register for notification from DataCollection service indicating it's connected/disconnected to network
    auto func = [this]( bool enabled )
    {
        if( enabled )
        {
            if( m_eedid.has_ediddata() )
            {
                m_DataCollectionClient->SendData( std::make_shared< DataCollectionPb::HdmiEdid >( m_eedid ), DATA_COLLECTION_EEDID );
            }
            if( m_cecStateCache.has_physicaladdress() )
            {
                m_DataCollectionClient->SendData( std::make_shared< DataCollectionPb::CecState >( m_cecStateCache ), DATA_COLLECTION_CEC_STATE );
            }
        }
    };
    m_DataCollectionClient->RegisterForEnabledNotifications( Callback<bool>( func ) );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::CecModeHandleGet
///
/// @brief  This method populates the supplied CecModeResponse argument
///
/// @param  CecModeResponse
///
/// @return
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::CecModeHandleGet( const Callback<const CecModeResponse> & resp, const Callback<FrontDoor::Error> & errorRsp )
{

    CecModeResponse cec = m_cecresp;
    SetCecModeDefaultProperties( cec );
    resp.Send( cec );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::CecModeHandlePut
///
/// @brief  This method handles an CecMode request
///
/// @param  CecUpdateRequest
///
/// @return
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::CecModeHandlePut( const CecUpdateRequest req, const Callback<const CecModeResponse> & resp, const Callback<FrontDoor::Error> & errorRsp )
{
    ProductMessage msg;
    FrontDoor::Error error;
    CecModeResponse tempResp;

    if( !req.has_mode() )
    {
        error.set_message( "Cec message has no mode." );
    }
    else if( req.mode() == s_ModeOn )
    {
        msg.mutable_cecmode()->set_cecmode( ProductCecMode::ON );
    }
    else if( req.mode() == s_ModeOff )
    {
        msg.mutable_cecmode()->set_cecmode( ProductCecMode::OFF );
    }
    else if( req.mode() == s_ModeAltOn )
    {
        msg.mutable_cecmode()->set_cecmode( ProductCecMode::ALTERNATE_ON );
    }
    else
    {
        error.set_message( "Cec message has invalid mode: " + req.mode( ) );
    }

    if( msg.has_cecmode() )
    {
        IL::BreakThread( [ = ]( )
        {
            m_ProductNotify( msg );
        }, m_ProductTask );

        m_cecresp.set_mode( req.mode( ) );
        CecModeHandleGet( resp, errorRsp );
    }
    else if( error.has_message( ) )
    {
        error.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        error.set_subcode( PGCErrorCodes::ERROR_SUBCODE_CEC );
        errorRsp.Send( error );
    }
    tempResp = m_cecresp;
    SetCecModeDefaultProperties( tempResp );
    PersistCecMode();
    m_FrontDoorClient->SendNotification( FRONTDOOR_CEC_API, tempResp );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductCecHelper::PerhapsSetCecSource
///
/// @brief Send the CEC source to the LPM, following the rules in PGC-1920
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::PerhapsSetCecSource( )
{
    if( !m_LpmPowerIsOn )
    {
        return;
    }

    if( !m_HavePhysicalAddress )
    {
        return;
    }

    if( m_LpmSourceID != LPM_IPC_SOURCE_STANDBY && // STANDBY is sent directly from PowerOff()
        m_LpmSourceID != LPM_IPC_INVALID_SOURCE )  // only send sensible value
    {
        SendCecSourceSelection( m_LpmSourceID );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductCecHelper::SendCecSourceSelection
///
/// @brief Send the CEC source to the LPM while maintaining hysteresis to avoid redundancy
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::SendCecSourceSelection( LPM_IPC_SOURCE_ID source )
{
    static LPM_IPC_SOURCE_ID lastSentSource = LPM_IPC_INVALID_SOURCE;
    if( source != lastSentSource )
    {
        lastSentSource = source;
        m_ProductLpmHardwareInterface->SendSourceSelection( source );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::SetCecModeDefaultProperties
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::SetCecModeDefaultProperties( ProductPb::CecModeResponse& cecResp )
{
    // fill in list of supported actions
    cecResp.mutable_properties()->add_supportedmodes( s_ModeOn );
    cecResp.mutable_properties()->add_supportedmodes( s_ModeOff );
    cecResp.mutable_properties()->add_supportedmodes( s_ModeAltOn );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::Connected
///
/// @param bool connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::Connected( bool connected )
{
    if( !connected )
    {
        BOSE_DEBUG( s_logger, "Connection to A4VVideoManager could not be established." );
        BOSE_DEBUG( s_logger, "An attempt to reconnect to A4VVideoManager will be made." );

        m_connected = false;

        IL::BreakThread( std::bind( &ProductCecHelper::Run, this ), m_ProductTask );

        return;
    }

    BOSE_DEBUG( s_logger, "A hardware connection to A4VVideoManager has been established." );
    BOSE_DEBUG( s_logger, "An attempt to register for HPD will now be made." );

    m_connected = true;

    Callback< A4VVideoManagerServiceMessages::EventHDMIMsg_t >
    CallbackForKeyEvents( std::bind( &ProductCecHelper::HandleHpdEvent,
                                     this,
                                     std::placeholders::_1 ) );

    m_CecHelper->RegisterForHotplugEvent( CallbackForKeyEvents );

    auto lpmConnectCb = [ this ]( bool connected )
    {
        const Callback< IPCSource_t > cecSrcSwitchCb( [ this ]( IPCSource_t source )
        {
            HandleSrcSwitch( source );
        } );
        m_ProductLpmHardwareInterface->RegisterForLpmEvents( IPC_ST_SOURCE, cecSrcSwitchCb );

        const Callback< IpcCecState_t > cecStateCb( [ this ]( IpcCecState_t state )
        {
            HandleCecState( state );
        } );
        m_ProductLpmHardwareInterface->RegisterForLpmEvents( static_cast< IpcOpcodes_t >( CEC_STATE_INFO ), cecStateCb );
    };
    m_ProductLpmHardwareInterface->RegisterForLpmConnection( lpmConnectCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductCecHelper::HandleSrcSwitch
///
/// @brief This method handles the CEC source switch message received from LPM
///
/// @param LpmServiceMessages::IPCSource_t cecSource
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleSrcSwitch( const LpmServiceMessages::IPCSource_t cecSource )
{
    BOSE_DEBUG( s_logger, "CEC Source Switch Message received from LPM  %d",  cecSource.source() );
    if( m_ignoreSourceSwitch )
    {
        BOSE_INFO( s_logger, "%s Ignoring CEC source switch", __PRETTY_FUNCTION__ );
        return;
    }

    ProductMessage productMessage;
    auto source = cecSource.source( );

    switch( source )
    {
    case LPM_IPC_SOURCE_TV:
        if( m_LpmSourceID == LPM_IPC_SOURCE_TV )
        {
            BOSE_INFO( s_logger, "Ignoring source switch to TV, already in cec source TV" );
            return;
        }
        productMessage.set_action( static_cast< uint32_t >( Action::ACTION_TV ) );
        break;

    case LPM_IPC_SOURCE_INTERNAL:
        productMessage.set_action( static_cast< uint32_t >( Action::POWER_TOGGLE ) );
        break;

    default:
        BOSE_ERROR( s_logger, "An invalid intent action has been supplied." );
        return;
    }

    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    BOSE_INFO( s_logger, "An attempt to play the %s has been made from CEC.", LPM_IPC_SOURCE_ID_Name( source ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::HandlePlaybackRequestResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandlePlaybackRequestResponse( const SoundTouchInterface::NowPlaying&
                                                      response )
{
    BOSE_DEBUG( s_logger, "A response to the playback request %s was received.",
                response.source( ).sourcedisplayname( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::HandlePlaybackRequestError
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandlePlaybackRequestError( const FrontDoor::Error& error )
{
    BOSE_WARNING( s_logger, "%s: Error = (%d-%d) %s", __func__, error.code(), error.subcode(), error.message().c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandleHpdEvent
///
/// @param A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleHpdEvent( A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent )
{
    BOSE_LOG( DEBUG, __PRETTY_FUNCTION__ );
    BOSE_LOG( INFO, "Got HDMI event : " << hpdEvent.event() );

    if( hpdEvent.event() == A4VVideoManagerServiceMessages::EventsHDMI_t::EHDMI_Connected )
    {
        {
            BOSE_LOG( INFO, "Sending edid raw Request" );
            auto func = std::bind(
                            &ProductCecHelper::HandleRawEDIDResponse,
                            this,
                            std::placeholders::_1 );
            AsyncCallback<A4VVideoManagerServiceMessages::EDIDRawMsg_t> cb( func, m_ProductTask );
            m_CecHelper->RequestRawEDID( cb );
        }
        {
            BOSE_LOG( INFO, "Sending Phy addr Request" );
            auto func = std::bind(
                            &ProductCecHelper::HandlePhyAddrResponse,
                            this,
                            std::placeholders::_1 );
            AsyncCallback<A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t> cb( func, m_ProductTask );
            m_CecHelper->RequestPhyAddr( cb );
        }

    }
    else
    {
        //disable physical address
        BOSE_DEBUG( s_logger, "CEC Physical address 0x%x is being set.", 0xffff );
        m_ProductLpmHardwareInterface->SetCecPhysicalAddress( 0xffff );

        m_HavePhysicalAddress = false;
        // Per PGC-1920, no CEC source sent to LPM when physical address is lost
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandleRawEDIDResponse
///
/// @param A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleRawEDIDResponse( A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid )
{
    //convert protobuf byte buffer to string
    std::stringstream stringEdid;
    const char *bytesBuf = rawEdid.edid().c_str();
    stringEdid << std::hex;
    stringEdid.fill( '0' );
    for( uint i = 0; i < rawEdid.edid().size(); ++i )
    {
        stringEdid << std::setw( 2 ) << ( int )bytesBuf[i];
    }

    m_eedid.set_ediddata( stringEdid.str() );

    m_DataCollectionClient->SendData( std::make_shared< DataCollectionPb::HdmiEdid >( m_eedid ), DATA_COLLECTION_EEDID );
    BOSE_DEBUG( s_logger, "ProductCecHelper::HandleRawEDIDResponse sent %s", m_eedid.ediddata().c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandlePhyAddrResponse
///
/// @param const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandlePhyAddrResponse( const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t cecPhysicalAddress )
{
    BOSE_DEBUG( s_logger, "CEC Physical address 0x%x is being set.", cecPhysicalAddress.addr() );

    if( m_connected == false || m_CecHelper == nullptr )
    {
        BOSE_ERROR( s_logger, "A send CEC PA request could not be made, as no connection is available." );

        return;
    }

    BOSE_DEBUG( s_logger, "A send CEC PA request will be made." );

    m_ProductLpmHardwareInterface->SetCecPhysicalAddress( cecPhysicalAddress.addr() );

    m_HavePhysicalAddress = true;
    PerhapsSetCecSource( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::Stop( )
{
    m_PutConnection.Disconnect();
    m_GetConnection.Disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandleNowPlaying
///
/// @param SoundTouchInterface::NowPlaying nowPlayingStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleNowPlaying( SoundTouchInterface::NowPlaying nowPlayingStatus )
{
    using namespace ProductSTS;

    BOSE_DEBUG( s_logger, "CEC CAPS now playing status has been received %s.", ProtoToMarkup::ToJson( nowPlayingStatus ).c_str( ) );

    // Default to allowing CEC source switch (we'll get nowPlaying with INVALID_SOURCE after setup, so we need to make sure
    // this defaults to cleared)
    m_ignoreSourceSwitch = false;

    if( nowPlayingStatus.state( ).status( ) == SoundTouchInterface::Status::PLAY )
    {
        if( nowPlayingStatus.has_container( )                          and
            nowPlayingStatus.container( ).has_contentitem( )           and
            nowPlayingStatus.container( ).contentitem( ).has_source( ) and
            nowPlayingStatus.container( ).contentitem( ).has_sourceaccount( ) )
        {
            const auto& source = nowPlayingStatus.container( ).contentitem( ).source( );
            if( source.compare( SHELBY_SOURCE::PRODUCT ) == 0 )
            {
                BOSE_DEBUG( s_logger, "CEC CAPS now playing source is set to SOURCE_TV." );

                m_LpmSourceID = LPM_IPC_SOURCE_TV;
            }
            else if( source.compare( SHELBY_SOURCE::SETUP ) == 0 )
            {
                BOSE_DEBUG( s_logger, "CEC CAPS in setup, ignoring CEC active source requests." );

                m_ignoreSourceSwitch = true;
                m_LpmSourceID = LPM_IPC_SOURCE_INTERNAL;
            }
            else
            {
                BOSE_DEBUG( s_logger, "CEC CAPS now playing source is set to LPM_IPC_SOURCE_INTERNAL." );

                m_LpmSourceID = LPM_IPC_SOURCE_INTERNAL;
            }
            PerhapsSetCecSource( );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandleFrontDoorVolume
///
/// @param  volume Object containing volume received from the FrontDoor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleFrontDoorVolume( SoundTouchInterface::volume const& volume )
{
    BOSE_VERBOSE( s_logger, "Got volume notify LPM (%d) (%d)", volume.value(), volume.muted() );

    m_ProductLpmHardwareInterface->NotifyVolumeMute( volume.value( ), volume.muted( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::PowerOff
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::PowerOff( )
{
    BOSE_INFO( s_logger, __func__ );

    A4VVideoManagerServiceMessages::PowerStateMsg_t msg;

    msg.set_state( A4VVideoManagerServiceMessages::PowerState_t::PS_Low );
    m_CecHelper->SetPowerState( msg );
    m_LpmSourceID = LPM_IPC_SOURCE_STANDBY;
    m_LpmPowerIsOn = false;

    // No need to defer to PerhapsSetCecSource(), we know we should be in STANDBY
    SendCecSourceSelection( LPM_IPC_SOURCE_STANDBY );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::PowerOn
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::PowerOn( )
{
    BOSE_INFO( s_logger, __func__ );

    A4VVideoManagerServiceMessages::PowerStateMsg_t msg;

    msg.set_state( A4VVideoManagerServiceMessages::PowerState_t::PS_Full );
    m_CecHelper->SetPowerState( msg );
    m_LpmPowerIsOn = true;

    PerhapsSetCecSource( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductCecHelper::HandleCecState
///
/// @brief This method handles the CEC state message received from LPM
///
/// @param LpmServiceMessages::IpcCecState_t state
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleCecState( const IpcCecState_t& state )
{
    // Presumably LPM sends these only on change, but there's no harm in filtering here just to be sure
    if( state.SerializeAsString() == m_cecState.SerializeAsString() )
    {
        return;
    }

    BOSE_DEBUG( s_logger, "%s - %s", __PRETTY_FUNCTION__, ProtoToMarkup::ToJson( state ).c_str( ) );

    m_cecStateCache.set_physicaladdress( state.physicaladdress( ) );
    m_cecStateCache.set_logicaladdress( state.logicaladdress( ) );
    m_cecStateCache.set_activesource( state.actsrc( ) );
    m_cecStateCache.set_strmpath( state.strmpath( ) );

    // Clear the list before copying over new items
    m_cecStateCache.mutable_cecdevices()->Clear();
    for( auto i = 0; i < state.cec_devices_size( ); i++ )
    {
        const auto& idev = state.cec_devices( i );
        auto odev = m_cecStateCache.add_cecdevices( );

        // TODO: do we need to filter this list based on LA (i.e. does the list
        // have a bunch of entries w/LA == CEC_UNREG_BCAST, and if so should we filter
        // them out?)

        odev->set_cecversion( CEC_VERSION_Name( idev.cecversion( ) ) );
        odev->set_physicaladdress( idev.physicaladdress( ) );
        odev->set_osd( idev.osd( ) );

        // should always be 3, but we'll be pedantic
        for( auto j = 0; j < idev.vendorid_size( ); j++ )
        {
            odev->add_vendorid( idev.vendorid( j ) );
        }
    }

    m_DataCollectionClient->SendData( std::make_shared< DataCollectionPb::CecState >( m_cecStateCache ), DATA_COLLECTION_CEC_STATE );
    m_cecState = state;
}

void ProductCecHelper::PersistCecMode()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    m_cecModePersistence->Remove();
    m_cecModePersistence->Store( ProtoToMarkup::ToJson( m_cecresp ) );
}

void ProductCecHelper::LoadFromPersistence()
{
    try
    {
        ProtoToMarkup::FromJson( m_cecModePersistence->Load(), &m_cecresp );
        BOSE_INFO( s_logger, "%s: %s",  __func__, ProtoToMarkup::ToJson( m_cecresp ).c_str() );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_LOG( ERROR, "CEC mode setting from persistence failed markup error - " << e.what() );
        m_cecresp.set_mode( "ON" ); //defaulting to ON
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException& e )
    {
        BOSE_LOG( ERROR, "CEC mode setting from persistence failed - " << e.what() );
        m_cecresp.set_mode( "ON" ); //defaulting to ON
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
