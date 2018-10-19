////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      RatingManager.cpp
///
/// @brief     This source code file declares a rating manager class for implementing rating control
///            (i.e. thumbs up/down) based on both product and remote-specific key actions.
///
/// @author    Nicholas Craffey
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
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
#include "FrontDoorClient.h"
#include "CustomProductController.h"
#include "RatingManager.h"
#include "ProductEndpointDefines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief RatingManager::RatingManager
///
/// @param NotifyTargetTaskIF&        task
///
/// @param const CliClientMT&         commandLineClient
///
/// @param const FrontDoorClientIF_t& frontDoorClient
///
/// @param ProductController&         productController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
RatingManager::RatingManager( NotifyTargetTaskIF&        task,
                              const CliClientMT&         commandLineClient,
                              const FrontDoorClientIF_t& frontDoorClient,
                              ProductController&         productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_CustomProductController( static_cast< CustomProductController & >( productController ) ),
      m_ProductTask( m_CustomProductController.GetTask( ) ),
      m_NotifierCallback( m_CustomProductController.GetMessageHandler( ) ),
      m_FrontDoorClient( frontDoorClient )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "RatingManager" );

    Initialize( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   RatingManager::Initialize
///
/// @brief  This method registers for rating notifications from the Front Door.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void RatingManager::Initialize( )
{
    auto fNotify = [ this ]( STS::Rating::CurrentRating r )
    {
        ReceiveFrontDoorRating( r );
    };

    m_NotifierCallback = m_FrontDoorClient->RegisterNotification< STS::Rating::CurrentRating >
                         ( FRONTDOOR_RATING_API, AsyncCallback< STS::Rating::CurrentRating > ( fNotify, m_ProductTask ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  RatingManager::Handle
///
/// @brief  This method is used to process rating specific key actions.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of the key action sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool RatingManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s is in %s handling the action %u.", "RatingManager",
               __func__, action );

    if( action == ( uint16_t )Action::ACTION_THUMB_DOWN )
    {
        ChangeRating( DOWN );
        return true;
    }

    if( action == ( uint16_t )Action::ACTION_THUMB_UP )
    {
        ChangeRating( UP );
        return true;
    }

    BOSE_ERROR( s_logger, "%s is in %s handling the unexpected action %u.", "RatingManager",
                __func__, action );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief RatingManager::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void RatingManager::Stop( )
{
    m_NotifierCallback.Disconnect( );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief RatingManager::ReceiveFrontDoorRating
///
/// @param  volume Object containing volume received from the FrontDoor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void RatingManager::ReceiveFrontDoorRating( STS::Rating::Enum& rating )
{
    BOSE_VERBOSE( s_logger, "rating received by %s is %s", __func__, ProtoToMarkup::ToJson( rating, false ).c_str() );
    ///
    /// Update rating status
    ///
    if ( rating == STS::Rating::Enum:UP )
    {
        m_rating = UP;
    }

    if ( rating == STS::Rating::Enum::DOWN )
    {
        m_rating = DOWN;
    }

    if ( rating == STS::Rating::Enum::NONE )
    {
        m_rating = NONE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name RatingManager::ChangeRating
///
/// @brief Changes the content rating to the given value
///
/// @param rating : one of { DOWN, NONE, UP } as defined in the RatingType enum
///                 and the STS::Rating::Enum protocol buffer
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void RatingManager::ChangeRating( RatingType rating )
{
    auto errFunc = []( FrontDoor::Error error )
    {
        BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a frontdoor rating request.",
                    error.code(),
                    error.subcode(),
                    error.message().c_str() );
    };
    auto respFunc = [ this ]( STS::Rating::Enum& r )
    {
        ReceiveFrontDoorRating( r );
    };

    AsyncCallback<STS::Rating::Enum&> respCb( respFunc, m_ProductTask );
    AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

    //STS::Rating::Enum& pbRating = ReceiveFrontDoorRating;
    //pbRating.set_rating( m_rating );

    BOSE_VERBOSE( s_logger, "Changing FrontDoor rating" );
    m_FrontDoorClient->SendPut<STS::Rating::Enum, FrontDoor::Error>(
        FRONTDOOR_RATING_API, pbRating, respCb, errCb );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
