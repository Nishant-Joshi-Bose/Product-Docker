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
#include "ProductMessage.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "InactivityTimers.h"

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
class ProductController;

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
                                  Hsm::STATE            stateId = 0,
                                  const std::string&    name    = "Top" );

    ~CustomProductControllerState( ) override
    {

    }

    void HandleStateEnter( ) override
    {

    }

    void HandleStateStart( ) override
    {

    }

    void HandleStateExit( ) override
    {

    }

    ///
    /// Methods are to be added at this point to implement product specific actions. These methods
    /// should be overridden in the product specific state derived class that actually executes
    /// product specific action.
    ///
    virtual bool HandleLpmState( bool active );
    virtual bool HandleCapsState( bool active );
    virtual bool HandleAudioPathState( bool active );
    virtual bool HandleSTSSourcesInit( );
    virtual bool HandleNetworkState( bool configured, bool connected );
    virtual bool HandleVoiceState( bool configured );
    virtual bool HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState state );
    virtual bool HandlePowerState( );
    virtual bool HandleAutowakeStatus( bool active );
    virtual bool HandleKeyAction( int action );
    virtual bool HandlePairingState( ProductAccessoryPairing pairingStatus );
    virtual bool HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo );
    virtual bool HandleInactivityTimer( InactivityTimerType timer );

    virtual bool HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo )
    {
        return false;
    }

    virtual bool HandleInactivityTimer( InactivityTimerType timerType )
    {
        return false;
    }

    static  void SetProductController( ProductController* productController )
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
