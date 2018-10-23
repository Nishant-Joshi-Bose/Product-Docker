////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      RatingManager.h
///
/// @brief     This header file declares a rating manager class for implementing rating control
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IntentManager.h"
#include "FrontDoorClientIF.h"
#include "SoundTouchInterface/Rating.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductController;
class CustomProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class RatingManager
///
/// @brief This class provides functionality to implement rating management.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class RatingManager : public IntentManager
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following enum represents the different types of ratings.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    enum RatingType
    {
        DOWN,
        NONE,
        UP
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Constructor for the RatingManager Class
    ///
    /// @param NotifyTargetTaskIF&         task
    ///
    /// @param const CliClientMT&          commandLineClient
    ///
    /// @param const FrontDoorClientIF_t&  frontDoorClient
    ///
    /// @param CustomProductController&    productController
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    RatingManager( NotifyTargetTaskIF&        task,
                   const CliClientMT&         commandLineClient,
                   const FrontDoorClientIF_t& frontDoorClient,
                   ProductController&         productController );


    ~RatingManager( ) override
    {

    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following method is used to handle rating actions (thumbs up or thumbs down).
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Handle( KeyHandlerUtil::ActionType_t& action ) override;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method allows for manipulation of content rating
    /// @param rating : one of { DOWN, UNRATED, UP } -- defined with the same values in the
    //                  RatingType enum of this class and the STS::Rating::Enum protobuf
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void ChangeRating( RatingType rating );

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable stores the custom product controller instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    //CustomProductController& GetCustomProductController( ) const;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods communicate with the FrontDoor to set and receive ratings.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void PostFrontDoorRatingResponse( const SoundTouchInterface::RatingTrack & rating );
    void PostFrontDoorRatingError( const FrontDoor::Error& error );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member tracks rating status.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    RatingType m_rating = NONE;

    ProductController&                      m_ProductController;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
