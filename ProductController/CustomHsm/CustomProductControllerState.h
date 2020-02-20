////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerState.h
///
/// @brief     This file declares a CustomProductControllerState class for executing produce
///            specific intents based on the currect product state and product specific events.
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
#include "NetManager.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "SystemEventMessage.pb.h"
#include "KeyManager.h"
#include "CustomProductControllerStates.h"
#include "SystemEventMessage.pb.h"
#include "ChimeEvents.h"
#include "CommonIntentHandler.h"

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
class CustomProductController;

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

#include "ProductControllerCommonEventHandlerDeclarations.cpp"

    virtual bool ShouldPlayVolumeTone( bool &canPlay )
    {
        canPlay = false;
        return false;
    }

    virtual bool IsInNetworkSetupState() const
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
    virtual bool HandlePairingStatus( ProductPb::AccessorySpeakerState pairingStatus )
    {
        return false;
    }

    virtual bool HandleModulesReady( )
    {
        return false;
    }

    virtual bool HandleLPMPowerStatusColdBoot( )
    {
        return false;
    }

    virtual bool HandleIntent( KeyHandlerUtil::ActionType_t intent )
    {
        return false;
    }

    virtual bool HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
    {
        return false;
    }

    virtual bool HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
    {
        return false;
    }

    virtual bool HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )
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

    virtual bool HandleIntentSetupBLERemote( )
    {
        return false;
    }

    virtual bool HandleIntentRating( KeyHandlerUtil::ActionType_t intent )
    {
        return false;
    }

    virtual bool HandleIntentAudioModeToggle( KeyHandlerUtil::ActionType_t intent )
    {
        return false;
    }

    virtual bool HandleIntentVoiceListening( )
    {
        return false;
    }

    virtual bool HandleAccessoriesAreKnown( )
    {
        return false;
    }

    virtual bool HandleDspBooted( const LpmServiceMessages::IpcDeviceBoot_t& dspBooted )
    {
        return false;
    }

    virtual bool HandleEthernetConnectionRemoved( )
    {
        return false;
    }

    virtual bool HandleOSMActivityState( bool state )
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// This method returns the Custom Product Controller reference of type
    /// CustomProductController.
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
    static CustomProductController& GetCustomProductController( );

    static void FrontDoorErrorCallback( const FrontDoor::Error & error );

    static ProductController* s_productController;

    static  KeyHandlerUtil::ActionType_t s_ActionPendingFromTransition;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
