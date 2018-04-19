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
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProductCecHelper.h"
#include "FrontDoorClient.h"
#include "EndPointsDefines.h"
#include "ProductEndpointDefines.h"
#include "PGCErrorCodes.h"
#include "DataCollectionClientFactory.h"
#include "HdmiEdid.pb.h"
#include "ProductDataCollectionDefines.h"

using namespace ProductPb;

namespace
{
const std::string s_ModeOn         = "On";
const std::string s_ModeOff        = "Off";
const std::string s_ModeAltOn      = "AltOn";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the VolumeManager
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char  FRONTDOOR_AUDIO_VOLUME[ ]           = "/audio/volume";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::ProductCecHelper
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCecHelper::ProductCecHelper( ProfessorProductController& ProductController )

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
      m_connected( false ),
      m_CustomProductController( static_cast< ProfessorProductController & >( ProductController ) ),
      m_DataCollectionClient( DataCollectionClientFactory::CreateUDCService( m_ProductTask ) )
{
    m_cecresp.set_mode( "On" );
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
    m_CecHelper = A4VVideoManagerClientFactory::Create( "ProductCecHelper", m_ProductTask );
    Callback< bool > ConnectedCallback( std::bind( &ProductCecHelper::Connected,
                                                   this,
                                                   std::placeholders::_1 ) );

    m_CecHelper->Connect( ConnectedCallback );

    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductCecHelper" );
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


    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        HandleFrontDoorVolume( v );
    };

    m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
    ( FRONTDOOR_AUDIO_VOLUME, fNotify );

    auto getFunc = [ this ]( const Callback< const CecModeResponse>& resp,
                             const Callback<FrontDoor::Error>& errorRsp )
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

    auto putFunc = [ this ]( const CecUpdateRequest                   cecReq,
                             const Callback< const CecModeResponse >& cecResp,
                             const Callback< FrontDoor::Error >&      errorRsp )
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

    if( !req.has_mode() )
    {
        error.set_message( "Cec message has no mode." );
    }
    else if( req.mode() == s_ModeOn )
    {
        msg.mutable_cecmode()->set_cecmode( ProductCecMode::On );
    }
    else if( req.mode() == s_ModeOff )
    {
        msg.mutable_cecmode()->set_cecmode( ProductCecMode::Off );
    }
    else if( req.mode() == s_ModeAltOn )
    {
        msg.mutable_cecmode()->set_cecmode( ProductCecMode::AltOn );
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
    else
    {
        BOSE_DEBUG( s_logger, "A hardware connection to A4VVideoManager has been established." );
        BOSE_DEBUG( s_logger, "An attempt to register for HPD will now be made." );

        m_connected = true;

        Callback< A4VVideoManagerServiceMessages::EventHDMIMsg_t >
        CallbackForKeyEvents( std::bind( &ProductCecHelper::HandleHpdEvent,
                                         this,
                                         std::placeholders::_1 ) );

        m_CecHelper->RegisterForHotplugEvent( CallbackForKeyEvents );

        Callback< LpmServiceMessages::IPCSource_t >
        CallbackForCecSource( std::bind( &ProductCecHelper::HandleSrcSwitch,
                                         this,
                                         std::placeholders::_1 ) );

        m_ProductLpmHardwareInterface->RegisterForLpmEvents( IPC_ST_SOURCE, CallbackForCecSource );

        return;
    }
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
    BOSE_DEBUG( s_logger, "Received CEC Source Switch message from LPM" );

    if( m_ProductLpmHardwareInterface == nullptr )
    {
        BOSE_ERROR( s_logger, "CEC Message could not be received, as no connection is available." );

        return;
    }
    else
    {

        BOSE_DEBUG( s_logger, "CEC Source Switch Message received from LPM  %d",  cecSource.source() );
        if( cecSource.source() == LPM_IPC_SOURCE_TV )
        {
            ProductMessage productMessage;
            productMessage.set_action( static_cast< uint32_t >( Action::ACTION_TV ) );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );

            BOSE_INFO( s_logger, "An attempt to play the TV source has been made from CEC." );
        }
        else if( cecSource.source() == LPM_IPC_SOURCE_INTERNAL )
        {
            ProductMessage productMessage;
            productMessage.set_action( static_cast< uint32_t >( Action::POWER_TOGGLE ) );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );

            BOSE_INFO( s_logger, "An attempt to play the last SoundTouch source has been made from CEC." );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid intent action has been supplied." );
        }

        return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCecHelper::HandlePlaybackRequestResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandlePlaybackRequestResponse( const SoundTouchInterface::NowPlaying&
                                                      response )
{
    BOSE_DEBUG( s_logger, "A response to the playback request %s was received." ,
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandleRawEDIDResponse
///
/// @param const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleRawEDIDResponse( const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid )
{
    BOSE_DEBUG( s_logger, "ProductCecHelper::SendEdidDataCollection" );

    auto edidData = std::make_shared< DataCollection::HdmiEdid >( );

    edidData->set_eedid( rawEdid.edid().c_str() );

    m_DataCollectionClient->SendData( edidData, DATA_COLLECTION_EEDID );
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
    else
    {
        BOSE_DEBUG( s_logger, "A send CEC PA request will be made." );

        m_ProductLpmHardwareInterface->SetCecPhysicalAddress( cecPhysicalAddress.addr() );

        return;
    }
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
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::HandleNowPlaying
///
/// @param SoundTouchInterface::NowPlaying& nowPlayingStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::HandleNowPlaying( const SoundTouchInterface::NowPlaying&
                                         nowPlayingStatus )
{
    BOSE_DEBUG( s_logger, "CEC CAPS now playing status has been received." );
    if( nowPlayingStatus.has_state( ) )
    {
        if( nowPlayingStatus.state( ).status( ) == SoundTouchInterface::Status::play )
        {
            if( nowPlayingStatus.has_container( )                          and
                nowPlayingStatus.container( ).has_contentitem( )           and
                nowPlayingStatus.container( ).contentitem( ).has_source( ) and
                nowPlayingStatus.container( ).contentitem( ).has_sourceaccount( ) )
            {
                if( nowPlayingStatus.container( ).contentitem( ).source( ).compare( "PRODUCT" ) == 0   and
                    nowPlayingStatus.container( ).contentitem( ).sourceaccount( ).compare( "TV" ) == 0 )
                {
                    BOSE_DEBUG( s_logger, "CEC CAPS now playing source is set to SOURCE_TV." );

                    m_ProductLpmHardwareInterface->SendSourceSelection( LPM_IPC_SOURCE_TV );
                }
                else
                {
                    BOSE_DEBUG( s_logger, "CEC CAPS now playing source is set to LPM_IPC_SOURCE_INTERNAL." );

                    m_ProductLpmHardwareInterface->SendSourceSelection( LPM_IPC_SOURCE_INTERNAL );
                }

            }
        }
    }
    else
    {
        BOSE_DEBUG( s_logger, "CEC CAPS now playing status is unknown. CEC STANDBY  %d",  LPM_IPC_SOURCE_STANDBY );
        m_ProductLpmHardwareInterface->SendSourceSelection( LPM_IPC_SOURCE_STANDBY );
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
    A4VVideoManagerServiceMessages::PowerStateMsg_t msg;

    msg.set_state( A4VVideoManagerServiceMessages::PowerState_t::PS_Low );
    m_CecHelper->SetPowerState( msg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductCecHelper::PowerOn
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCecHelper::PowerOn( )
{
    A4VVideoManagerServiceMessages::PowerStateMsg_t msg;

    msg.set_state( A4VVideoManagerServiceMessages::PowerState_t::PS_Full );
    m_CecHelper->SetPowerState( msg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
