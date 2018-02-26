////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      IntentHandler.h
///
/// @brief     This header file declares an IntentHandler class that implements an intent handler.
///            Its purpose is to offload the work of the product controller, validate, and build
///            LAN API or IPC messages to perform actions of various intents. It also lets the
///            product controller get control back in an async way, so the HSM can perform state
///            changes if it needs to.
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
#include "FrontDoorClientIF.h"
#include "CommonIntentHandler.h"
#include "Intents.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CliClientMT;

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
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The IntentHandler Class
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class IntentHandler: public CommonIntentHandler
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief IntentHandler
    ///
    /// @param NotifyTargetTaskIF&        task
    ///
    /// @param const CliClientMT&         commandLineClient
    ///
    /// @param const FrontDoorClientIF_t& frontDoorClient
    ///
    /// @param ProductController&         productController
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    IntentHandler( NotifyTargetTaskIF&        task,
                   const CliClientMT&         commandLineClient,
                   const FrontDoorClientIF_t& frontDoorClient,
                   ProductController&         productController );

    ~IntentHandler( ) override
    {

    }

    ///
    /// Initialization will include adding product specific intent managers that are associated with
    /// intent values.
    ///
    void Initialize( ) override;

    ///
    /// The following static member functions are used to determine whether the key action supplied
    /// as an argument to the method belongs to a particular product specific intent.
    ///
    static bool IsIntentUserPower( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( uint16_t )Action::ACTION_POWER ) ;
    }

    static bool IsIntentMuteControl( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( uint16_t )Action::ACTION_MUTE );
    }

    static bool IsIntentSpeakerPairing( KeyHandlerUtil::ActionType_t argument )
    {
        if( argument == ( uint16_t )Action::ACTION_LPM_PAIR_SPEAKERS )
        {
            return true;
        }
        if( argument == ( uint16_t )Action::ACTION_START_PAIR_SPEAKERS )
        {
            return true;
        }
        if( argument == ( uint16_t )Action::ACTION_STOP_PAIR_SPEAKERS )
        {
            return true;
        }
        return false;

    }

    static bool IsIntentPlayProductSource( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( uint16_t )Action::ACTION_TV );
    }

    static bool IsIntentPlaySoundTouchSource( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( uint16_t )Action::ACTION_SOUNDTOUCH );
    }

    static bool IsIntentPlayCustomSource( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( uint16_t )Action::ACTION_CUSTOM_GAME ) ||
               ( argument == ( uint16_t )Action::ACTION_CUSTOM_DVD ) ||
               ( argument == ( uint16_t )Action::ACTION_CUSTOM_CABLESAT );
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
