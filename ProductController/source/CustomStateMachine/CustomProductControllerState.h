////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerState.h
///
/// @brief     This file declares a CustomProductControllerState class for executing produce
///            specific actions based on the currect product state and product specific events.
///
/// @author    Stuart J. Lumby
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
#include "Hsm.h"
#include "HsmState.h"
#include "InactivityTimers.h"
#include "ProductMessage.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "KeyManager.h"
#include "CustomProductControllerStates.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductControllerHsm;
class ProductController;
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductControllerState
///
/// @brief This class is used for executing produce specific actions based on the currect product
///        state and product specific events.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerState : public CHsmState
{

public:

    CustomProductControllerState( ProductControllerHsm& productStateMachine,
                                  CHsmState*            pSuperState,
                                  Hsm::STATE            stateId,
                                  const std::string&    name );

    ~CustomProductControllerState( ) override
    {

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// Here are the common event handlers. They need to be declared virtual and have an
    /// implementation that returns false.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool HandleLpmState( bool isActive )
    {
        return false;
    }

    virtual bool HandleCapsState( bool ready )
    {
        return false;
    }

    virtual bool HandleAudioPathState( bool ready )
    {
        return false;
    }

    virtual bool HandleNetworkState( bool configured, bool connected )
    {
        return false;
    }

    virtual bool HandleVoiceState( bool configured )
    {
        return false;
    }

    virtual bool HandleAutowakeStatus( bool active )
    {
        return false;
    }

    virtual bool HandleIntentPlayControl( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentBlueTooth( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentVolumeControl( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentNetworkStandby( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentFactoryDefault( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentPresetSelect( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentPresetStore( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentLowPowerStandby( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }
    virtual bool HandleIntentVoice( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentWiFi( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleBluetoothModuleState( bool ready )
    {
        return false;
    }

    virtual bool HandleSTSSourcesInit( )
    {
        return false;
    }

    virtual bool HandleLpmInterfaceState( bool isConnected )
    {
        return false;
    }

    virtual bool HandleLpmLowpowerSystemState()
    {
        return false;
    }

    virtual bool HandleLpmLowPowerStatus( const ProductLpmLowPowerStatus& )
    {
        return false;
    }
    virtual bool HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo )
    {
        return false;
    }

    virtual bool HandleNowPlayingStatus( const ProductNowPlayingStatus_ProductNowPlayingState& state )
    {
        return false;
    }

    virtual bool HandleInactivityTimer( InactivityTimerType timerType )
    {
        return false;
    }

    virtual bool HandleStopPlaybackResponse( const SoundTouchInterface::status &resp )
    {
        return false;
    }

    virtual bool HandleFactoryDefault( )
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// Methods are to be added at this point to implement product specific actions. These methods
    /// should be overridden in the product specific state derived class that actually executes
    /// product specific action.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool HandlePairingState( ProductAccessoryPairing pairingStatus )
    {
        return false;
    }

    virtual bool HandleModulesReady( )
    {
        return false;
    }

    virtual bool HandleNetworkConfigurationStatus( )
    {
        return false;
    }

    virtual bool HandleLPMPowerStatusColdBoot( )
    {
        return false;
    }

    virtual bool HandleLPMPowerStatusLowPower( )
    {
        return false;
    }

    virtual bool HandleLPMPowerStatusNetworkStandby( )
    {
        return false;
    }

    virtual bool HandleLPMPowerStatusAutoWakeStandby( )
    {
        return false;
    }

    virtual bool HandleLPMPowerStatusFullPower( )
    {
        return false;
    }

    virtual bool HandleIntent( KeyHandlerUtil::ActionType_t action )
    {
        return false;
    }

    virtual bool HandleIntentUserPower( KeyHandlerUtil::ActionType_t action )
    {
        return false;
    }

    virtual bool HandleIntentVolumeMuteControl( KeyHandlerUtil::ActionType_t action )
    {
        return false;
    }

    virtual bool HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t action )
    {
        return false;
    }

    virtual bool HandleIntentPlayback( KeyHandlerUtil::ActionType_t action )
    {
        return false;
    }

    virtual bool HandleAdaptIQStatus( const ProductAdaptIQStatus& status )
    {
        return false;
    }

    virtual bool HandleAdaptIQControl( const ProductAdaptIQControl& control )
    {
        return false;
    }

    virtual bool HandleAudioPathSelect( )
    {
        return false;
    }

    virtual bool HandleAudioPathDeselect( )
    {
        return false;
    }

    /*! \brief Respond to the LPM detecting an amp fault.
     */
    virtual bool HandleAmpFaultDetected()
    {
        return false;
    }

    virtual bool HandleSoftwareUpdateStatus( )
    {
        return false;
    }

    virtual bool IsInNetworkSetupState() const
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// This method returns the Professor Custom Product Controller reference of type
    /// ProfessorProductController.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline static void SetProductController( ProductController* productController )
    {
        s_productController = productController;
    }

protected:
    ///
    /// The custom version of this function returns the custom ProductController.
    ///
    static ProfessorProductController& GetCustomProductController( );

    static ProductController* s_productController;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
