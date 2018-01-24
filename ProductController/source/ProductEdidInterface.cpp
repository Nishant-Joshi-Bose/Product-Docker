////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductEdidInterface.cpp
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
#include "ProductEdidInterface.h"
#include "FrontDoorClient.h"

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
/// @name   ProductEdidInterface::ProductEdidInterface
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductEdidInterface::ProductEdidInterface( ProfessorProductController& ProductController )

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
      m_connected( false )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductEdidInterface::Run
///
/// @brief  This method connects and starts the handling of communication with A4VVideoManager
///         service.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductEdidInterface::Run( )
{
    BOSE_DEBUG( s_logger, "The hardware connection to the A4VVideoManager is being established." );
    m_EdidClient = A4VVideoManagerClientFactory::Create( "ProductEdidInterface", m_ProductTask );
    Callback< bool > ConnectedCallback( std::bind( &ProductEdidInterface::Connected,
                                                   this,
                                                   std::placeholders::_1 ) );

    m_EdidClient->Connect( ConnectedCallback );

    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductEdidInterface" );
    ///
    /// Registration as a client for getting notification of changes in the now playing state from
    /// CAPS is made through the FrontDoorClient object pointer. The callback HandleCapsNowPlaying
    /// is used to receive these notifications.
    ///
    AsyncCallback< SoundTouchInterface::NowPlaying >
    callback( std::bind( &ProductEdidInterface::HandleNowPlaying,
                         this, std::placeholders::_1 ),
              m_ProductTask );

    m_FrontDoorClient->RegisterNotification< SoundTouchInterface::NowPlaying >
    ( "/content/nowPlaying", callback );


    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        HandleFrontDoorVolume( v );
    };

    m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
    ( FRONTDOOR_AUDIO_VOLUME, fNotify );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::Connected
///
/// @param bool connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::Connected( bool connected )
{
    if( !connected )
    {
        BOSE_DEBUG( s_logger, "Connection to A4VVideoManager could not be established." );
        BOSE_DEBUG( s_logger, "An attempt to reconnect to A4VVideoManager will be made." );

        m_connected = false;

        IL::BreakThread( std::bind( &ProductEdidInterface::Run, this ), m_ProductTask );

        return;
    }
    else
    {
        BOSE_DEBUG( s_logger, "A hardware connection to A4VVideoManager has been established." );
        BOSE_DEBUG( s_logger, "An attempt to register for HPD will now be made." );

        m_connected = true;

        Callback< A4VVideoManagerServiceMessages::EventHDMIMsg_t >
        CallbackForKeyEvents( std::bind( &ProductEdidInterface::HandleHpdEvent,
                                         this,
                                         std::placeholders::_1 ) );

        m_EdidClient->RegisterForHotplugEvent( CallbackForKeyEvents );

        return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandleHpdEvent
///
/// @param A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandleHpdEvent( A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent )
{
    BOSE_LOG( DEBUG, __PRETTY_FUNCTION__ );
    BOSE_LOG( INFO, "Got HDMI event : " << hpdEvent.event() );

    if( hpdEvent.event() == A4VVideoManagerServiceMessages::EventsHDMI_t::EHDMI_Connected )
    {
        {
            BOSE_LOG( INFO, "Sending edid raw Request" );
            auto func = std::bind(
                            &ProductEdidInterface::HandleRawEDIDResponse,
                            this,
                            std::placeholders::_1 );
            AsyncCallback<A4VVideoManagerServiceMessages::EDIDRawMsg_t> cb( func, m_ProductTask );
            m_EdidClient->RequestRawEDID( cb );
        }
        {
            BOSE_LOG( INFO, "Sending Phy addr Request" );
            auto func = std::bind(
                            &ProductEdidInterface::HandlePhyAddrResponse,
                            this,
                            std::placeholders::_1 );
            AsyncCallback<A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t> cb( func, m_ProductTask );
            m_EdidClient->RequestPhyAddr( cb );
        }

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandleRawEDIDResponse
///
/// @param const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandleRawEDIDResponse( const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid )
{
    //TBD - Mano
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandlePhyAddrResponse
///
/// @param const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandlePhyAddrResponse( const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t cecPhysicalAddress )
{
    BOSE_DEBUG( s_logger, "CEC Physical address 0x%x is being set.", cecPhysicalAddress.addr() );

    if( m_connected == false || m_EdidClient == nullptr )
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
/// @brief ProductEdidInterface::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::Stop( )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandleNowPlaying
///
/// @param SoundTouchInterface::NowPlaying& nowPlayingStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandleNowPlaying( const SoundTouchInterface::NowPlaying&
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
                    BOSE_DEBUG( s_logger, "CEC CAPS now playing source is set to SOURCE_SOUNDTOUCH." );

                    m_ProductLpmHardwareInterface->SendSourceSelection( LPM_IPC_SOURCE_SHELBY );
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
/// @brief ProductEdidInterface::HandleFrontDoorVolume
///
/// @param  volume Object containing volume received from the FrontDoor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandleFrontDoorVolume( SoundTouchInterface::volume const& volume )
{
    BOSE_VERBOSE( s_logger, "Got volume notify LPM (%d) (%d)", volume.value(), volume.muted() );

    m_ProductLpmHardwareInterface->NotifyVolumeLevel( volume.value( ) );
    m_ProductLpmHardwareInterface->NotifyMuteState( volume.muted( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::PowerOff
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::PowerOff( )
{
    A4VVideoManagerServiceMessages::PowerStateMsg_t msg;

    msg.set_state( A4VVideoManagerServiceMessages::PowerState_t::PS_Low );
    m_EdidClient->SetPowerState( msg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::PowerOn
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::PowerOn( )
{
    A4VVideoManagerServiceMessages::PowerStateMsg_t msg;

    msg.set_state( A4VVideoManagerServiceMessages::PowerState_t::PS_Full );
    m_EdidClient->SetPowerState( msg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
