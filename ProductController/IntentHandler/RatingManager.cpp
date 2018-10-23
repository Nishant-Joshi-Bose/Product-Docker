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
#include "RatingManager.h"
#include "CustomProductController.h"
#include "Intents.h"
#include "ProductEndpointDefines.h"
#include "ProductSTS.pb.h"

using namespace ProductPb;

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
      m_ProductController( productController )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "RatingManager" );

}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  RatingManager::Handle
///
/// @brief  This method is used to build and send Front Door messages to rate a source
///         if it is selected.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of the key action sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool RatingManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s in %s", "RatingManager", __func__ );

    using namespace ProductSTS;

    auto ratingMangerResponseCallback = [ this ]( const SoundTouchInterface::RatingTrack & response )
    {
        BOSE_DEBUG( s_logger, "A response to the rating was received: %s",
                    ProtoToMarkup::ToJson( response, false ).c_str( ) );
    };

    auto ratingMangerErrorCallback = [ this ]( const FrontDoor::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a rating request.",
                    error.code(),
                    error.subcode(),
                    error.message().c_str() );
    };

    SoundTouchInterface::RatingTrack ratingData;

    if( action == ( uint16_t )Action::ACTION_THUMB_UP )
    {
        ratingData.set_rating( SoundTouchInterface::Rating::RatingEnum::UP );
    }

    if( action == ( uint16_t )Action::ACTION_THUMB_DOWN )
    {
        ratingData.set_rating( SoundTouchInterface::Rating::RatingEnum::DOWN );
    }

    GetFrontDoorClient( )->SendPost<SoundTouchInterface::RatingTrack, FrontDoor::Error>( FRONTDOOR_RATING_API,
            ratingData,
            ratingMangerResponseCallback,
            ratingMangerErrorCallback );

    return true;
}

//CustomProductController& RatingManager::GetCustomProductController( )
//{
//    return static_cast<CustomProductController&>( m_ProductController );
//}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
