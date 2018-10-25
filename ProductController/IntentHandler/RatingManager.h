////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      RatingManager.h
/// @brief     This header file declares a rating manager class for implementing rating control
///            (i.e. thumbs up/down) based on both product and remote-specific key actions.
/// @author    Nicholas Craffey
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IntentManager.h"
#include "Intents.h"
#include "FrontDoorClientIF.h"
#include "SoundTouchInterface/Rating.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "Utilities.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class RatingManager
/// @brief This class provides functionality to implement rating management.
////////////////////////////////////////////////////////////////////////////////////////////////////
class RatingManager : public IntentManager
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The following enum represents the different types of ratings.
    //////////////////////////////////////////////////////////////////////////////////////////////
    enum RatingType
    {
        DOWN,
        NONE,
        UP
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Constructor for the RatingManager Class
    /// @param NotifyTargetTaskIF&         task
    /// @param const CliClientMT&          commandLineClient
    /// @param const FrontDoorClientIF_t&  frontDoorClient
    /// @param CustomProductController&    productController
    ////////////////////////////////////////////////////////////////////////////////////////////////
    RatingManager( NotifyTargetTaskIF&        task,
                   const CliClientMT&         commandLineClient,
                   const FrontDoorClientIF_t& frontDoorClient,
                   ProductController&         productController );


    ~RatingManager( ) override
    {

    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief  The following method is used to handle rating actions (thumbs up or thumbs down).
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Handle( KeyHandlerUtil::ActionType_t& action ) override;

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The following methods communicate with the FrontDoor to set and receive ratings.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void PostFrontDoorRatingResponse( const SoundTouchInterface::RatingTrack & rating );
    void PostFrontDoorRatingError( const FrontDoor::Error& error );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The following member tracks rating status.
    //////////////////////////////////////////////////////////////////////////////////////////////
    RatingType m_rating = NONE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
