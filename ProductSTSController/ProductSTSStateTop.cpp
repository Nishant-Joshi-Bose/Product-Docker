////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTop.cpp
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSStateTop.h"
#include "ProductSTSController.h"
#include "ProductSTSIntrospect.pb.h"
#include "ProtoToMarkup.h"

//////////////////////////////////////////////////////////////
ProductSTSStateTop::ProductSTSStateTop( ProductSTSHsm& hsm,
                                        CHsmState *pSuperState,
                                        ProductSTSAccount& account ) :
    STSState<ProductSTSStateTop>( hsm,
                                  pSuperState,
                                  0,
                                  "Top" ),
    m_logger( "ProductSTSState" ),
    m_account( account ),
    m_active( false )
{
}

bool ProductSTSStateTop::HandleSelectContentItem( const STS::ContentItem& ci )
{
    BOSE_INFO( m_logger, "HandleSelectContentItem( %s )", m_account.GetSourceName().c_str() );
    m_account.IPC().SendSourceSelectEvent();
    *( m_np.mutable_contentitem() ) = ci;
    m_np.mutable_contentitem()->set_ispresetable( false );
    m_account.s_ProductSTSController->HandleSelectSourceSlot( m_account.GetSourceID() );
    return true;
}

bool ProductSTSStateTop::HandleActivateRequest( const STS::Void &, uint32_t seq )
{
    BOSE_INFO( m_logger, "HandleActivateRequest( %s )", m_account.GetSourceName().c_str() );
    m_account.IPC().SendActivateResponse( seq );

    // In ECO2 the audio path setting is made via the APProduct interface
    // A4VServerTask::getInstance()->SendActivateSource( m_account.GetSourceID() );

    m_np.set_playstatus( STS::PlayStatus::PLAY );
    STS::NowPlayingChange npc;
    *( npc.mutable_nowplaying() ) = m_np;
    m_account.IPC().SendNowPlayingChangeEvent( npc );
    m_active = true;
    return true;
}

bool ProductSTSStateTop::HandleNowPlayingRequest( const STS::Void &, uint32_t seq )
{
    BOSE_INFO( m_logger, "HandleNowPlayingRequest( %s ) while %sactive", m_account.GetSourceName().c_str(),
               m_active ? "" : "in" );
    if( m_active )
    {
        STS::NowPlayingResponse npr;
        *( npr.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingResponse( npr, seq );
    }
    else
    {
        STS::AsyncError e;
        e.set_error( SoundTouchInterface::ErrorType::MUSIC_SERVICE_WRONG_STATE_NOW_PLAYING_REQUEST );
        e.set_desc( m_account.GetSourceName() + "RcvdNowPlayingInWrongState" );
        m_account.IPC().SendNowPlayingFailureResponse( e, seq );
    }
    return true;
}

bool ProductSTSStateTop::HandleDeactivateRequest( const STS::DeactivateRequest &, uint32_t seq )
{
    BOSE_INFO( m_logger, "HandleDeactivateRequest( %s )", m_account.GetSourceName().c_str() );
    m_account.IPC().SendAudioStopEvent();
    m_account.IPC().SendDeactivateResponse( seq );
    m_np.set_playstatus( STS::PlayStatus::STOP );
    m_active = false;
    return true;
}

////////////////////////////////////////////////////////
bool ProductSTSStateTop::HandleIntrospectRequest( const STS::Void&, uint32_t seq )
{
    m_account.IPC().SendIntrospectResponse( ProtoToMarkup::ToXML( m_account.Introspect() ) , seq );
    return true;
}

