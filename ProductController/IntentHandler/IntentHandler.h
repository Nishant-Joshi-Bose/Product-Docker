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
class CustomProductController;

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
    static bool IsIntentMuteControl( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_MUTE ||
                 argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_ASSERT_MUTE ||
                 argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_ASSERT_UNMUTE );
    }

    static bool IsIntentSpeakerPairing( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_LPM_PAIR_SPEAKERS ||
                 argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_START_PAIR_SPEAKERS_LAN ||
                 argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_START_PAIR_SPEAKERS ||
                 argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_STOP_PAIR_SPEAKERS );
    }

    static bool IsIntentPlayProductSource( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_TV ) ||
               ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_GAME ) ||
               ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_DVD ) ||
               ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_CABLESAT );
    }

    static bool IsIntentPlaySoundTouchSource( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_SOUNDTOUCH );
    }

    static bool IsIntentSetupBLERemote( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_SETUP_BLE_REMOTE );
    }

    static bool IsIntentRating( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_THUMB_UP ) ||
               ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_THUMB_DOWN );
    }

    static bool IsIntentAudioModeToggle( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_AUDIO_MODE_TOGGLE );
    }

    static bool IsIntentBootupFactoryDefault( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::BOOTUP_FACTORY_DEFAULT_START ) ||
               ( argument == ( KeyHandlerUtil::ActionType_t )Action::BOOTUP_FACTORY_DEFAULT_COMPLETE );
    }

    static bool IsIntentVoiceListening( KeyHandlerUtil::ActionType_t argument )
    {
        return ( argument == ( KeyHandlerUtil::ActionType_t )Action::ACTION_VOICE_LISTENING );
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
