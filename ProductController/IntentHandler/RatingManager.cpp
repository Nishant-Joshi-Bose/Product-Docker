////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      RatingManager.cpp
/// @brief     This source code file declares a rating manager class for implementing rating control
///            (i.e. thumbs up/down) based on both product and remote-specific key actions.
/// @author    Nicholas Craffey
////////////////////////////////////////////////////////////////////////////////////////////////////;

#include "RatingManager.h"
#include "ProductEndpointDefines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief RatingManager::RatingManager
/// @param NotifyTargetTaskIF&        task
/// @param const CliClientMT&         commandLineClient
/// @param const FrontDoorClientIF_t& frontDoorClient
/// @param ProductController&         productController
////////////////////////////////////////////////////////////////////////////////////////////////////
RatingManager::RatingManager( NotifyTargetTaskIF&        task,
                              const CliClientMT&         commandLineClient,
                              const FrontDoorClientIF_t& frontDoorClient,
                              ProductController&         productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "RatingManager" );

}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  RatingManager::Handle
/// @brief  This method is used to build and send Front Door messages to rate a source
///         if it is selected.
/// @param  KeyHandlerUtil::ActionType_t& action
/// @return This method returns true base on its handling of the key action sent.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool RatingManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s in %s", "RatingManager", __func__ );

    auto ratingMangerErrorCallback = [ this ]( const FrontDoor::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a rating request.",
                    error.code(),
                    error.subcode(),
                    error.message().c_str() );
    };

    CAPSAPI::RatingTrack ratingData;

    if( action == ( uint16_t )Action::ACTION_THUMB_UP )
    {
        ratingData.set_rating( CAPSAPI::Rating::RatingEnum::UP );
    }
    else if( action == ( uint16_t )Action::ACTION_THUMB_DOWN )
    {
        ratingData.set_rating( CAPSAPI::Rating::RatingEnum::DOWN );
    }
    else
    {
        BOSE_ERROR( s_logger, "Unexpected intent %d received", action );
        return false;
    }

    GetFrontDoorClient( )->SendPost<CAPSAPI::RatingTrack, FrontDoor::Error>( FRONTDOOR_RATING_API,
                                                                             ratingData,
                                                                             {},
                                                                             ratingMangerErrorCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
