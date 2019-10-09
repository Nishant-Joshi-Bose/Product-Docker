////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      IntentHandler.cpp
///
/// @brief     This file contains source code that implements an intent handler. Its purpose is to
///            offload the work of the product controller, validate, and build LAN API or IPC
///            messages to perform actions of various intents. It also lets the product controller
///            get control back in an async way, so the HSM can perform state changes if it needs
///            to.
///
/// @note      This Handler should not perform state transistions. It is intentional that HSM or
///            product controller access is not given to this module. The handler in turn should
///            delegate its work to customized intent managers based on the ActionType that is
///            passed to it. These customized intent managers will be taking actions based on the
///            following:
///
///            1. State of its own subsystem, based on what was processed for the same action
///               before, like a play or pause would have to toggle the actions.
///
///            2. The intent handler will call various intent managers that are registered for
///               specific intents. The initializing of intent managers needs to be done in the
///               method IntentHandler::Initialize for all intents that needs to be handled by this
///               module.
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
#include "IntentHandler.h"
#include "CustomProductController.h"
#include "CliClientMT.h"
#include "MuteManager.h"
#include "SpeakerPairingManager.h"
#include "PlaybackRequestManager.h"
#include "RatingManager.h"
#include "AudioModeManager.h"
#include "BootupFactoryDefaultManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief IntentHandler::IntentHandler
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
IntentHandler::IntentHandler( NotifyTargetTaskIF&         task,
                              const CliClientMT&          commandLineClient,
                              const FrontDoorClientIF_t&  frontDoorClient,
                              ProductController&          productController )

    : CommonIntentHandler( task, commandLineClient, frontDoorClient, productController )
{
    BOSE_DEBUG( s_logger, "The IntentHandler is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief IntentHandler::Initialize
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void IntentHandler::Initialize( )
{
    BOSE_DEBUG( s_logger, "IntentHandler is in %s.", __func__ );

    ///
    /// Initialize all the common intents found in the common source code.
    ///
    CommonIntentHandler::Initialize( );

    ///
    /// Custom intent managers are constructed at this point.
    ///
    IntentManagerPtr_t muteManager = std::make_shared< MuteManager >
                                     ( GetTask( ),
                                       GetCli( ),
                                       GetFrontDoorClient( ),
                                       GetProductController( ) );

    IntentManagerPtr_t speakerPairingManager = std::make_shared< SpeakerPairingManager >
                                               ( GetTask( ),
                                                 GetCli( ),
                                                 GetFrontDoorClient( ),
                                                 GetProductController( ) );

    IntentManagerPtr_t playbackRequestManager = std::make_shared< PlaybackRequestManager >
                                                ( GetTask( ),
                                                  GetCli( ),
                                                  GetFrontDoorClient( ),
                                                  GetProductController( ) );

    IntentManagerPtr_t ratingManager = std::make_shared< RatingManager >
                                       ( GetTask( ),
                                         GetCli( ),
                                         GetFrontDoorClient( ),
                                         GetProductController( ) );

    IntentManagerPtr_t audioModeManager = std::make_shared< AudioModeManager >
                                          ( GetTask( ),
                                            GetCli( ),
                                            GetFrontDoorClient( ),
                                            GetProductController( ) );

    IntentManagerPtr_t bootupFactoryDefaultManager = std::make_shared< BootupFactoryDefaultManager >
                                                     ( GetTask( ),
                                                       GetCli( ),
                                                       GetFrontDoorClient( ),
                                                       GetProductController( ) );

    ///
    /// A map is created to associate the custom volume and mute control intent manager with volume
    /// and mute key actions. Note that these actions are product specific,
    /// typically based on remote key actions.
    ///
    m_IntentManagerMap[( uint16_t )Action::ACTION_MUTE ] = muteManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_ASSERT_MUTE ] = muteManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_ASSERT_UNMUTE ] = muteManager;

    ///
    /// A map is created to associate the custom speaker pairing intent manager with pair speaker key
    /// actions.
    ///
    m_IntentManagerMap[( uint16_t )Action::ACTION_START_PAIR_SPEAKERS ]     = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_START_PAIR_SPEAKERS_LAN ] = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_LPM_PAIR_SPEAKERS ]       = speakerPairingManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_STOP_PAIR_SPEAKERS ]      = speakerPairingManager;

    ///
    /// A map is created to associate the custom playback intent manager with product specific
    /// source selection key actions, typically based on remote key actions.
    ///
    m_IntentManagerMap[( uint16_t )Action::ACTION_TV ]              = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_TV_CEC ]          = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_APAPTIQ_START ]   = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_GAME ]            = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_DVD ]             = playbackRequestManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_CABLESAT ]        = playbackRequestManager;

    ///
    /// A map is created to associate the custom rating intent manager with product specific
    /// rating key actions - i.e. a "thumbs up" or "thumbs down" key found on various universal
    /// remotes.
    ///
    m_IntentManagerMap[( uint16_t )Action::ACTION_THUMB_UP ]    = ratingManager;
    m_IntentManagerMap[( uint16_t )Action::ACTION_THUMB_DOWN ]  = ratingManager;

    ///
    /// A map is created to associate the custom audio mode intent manager with a key action
    /// which toggles the audio mode.
    ///
    m_IntentManagerMap[( uint16_t )Action::ACTION_AUDIO_MODE_TOGGLE ] = audioModeManager;

    ///
    /// A map is created to associate the custom BootupFactoryDefault intent manager with the key actions
    /// which participate in triggering a FactoryDefault at bootup time
    ///
    m_IntentManagerMap[( uint16_t )Action::BOOTUP_FACTORY_DEFAULT_START ] = bootupFactoryDefaultManager;
    m_IntentManagerMap[( uint16_t )Action::BOOTUP_FACTORY_DEFAULT_COMPLETE ] = bootupFactoryDefaultManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
