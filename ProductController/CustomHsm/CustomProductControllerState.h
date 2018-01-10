////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerState.h
/// @brief  Class for executing product specific event handlers based on
///         product specific events. Home for all product specific event
///         handlers that should be performed within product specific/custom
///         states. This class also holds pure virtual methods for common
///         event handlers.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Hsm.h"
#include "HsmState.h"
#include "KeyHandler.h"
#include "NetManager.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "ProductMessage.pb.h"
#include "InactivityTimers.h"

namespace ProductApp
{
class EddieProductController;
class ProductController;
class ProductControllerHsm;

class CustomProductControllerState : public CHsmState
{
public:
    CustomProductControllerState( ProductControllerHsm& hsm, CHsmState* pSuperState,
                                  Hsm::STATE stateId = 0, const std::string& name = "Top" );

    ~CustomProductControllerState() override
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    ///
    /// Here are the common event handlers. They need to be declared virtual and
    /// have an implementation that returns false
    ///
    ///////////////////////////////////////////////////////////////////////////////
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

    virtual bool HandleIntentLowPowerStandby( KeyHandlerUtil::ActionType_t intent )
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

    virtual bool HandleIntentVoice( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleIntentWiFi( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }
    virtual bool HandleIntentVolume( KeyHandlerUtil::ActionType_t result )
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

    ///////////////////////////////////////////////////////////////////////////////
    ///
    /// Here are the Custom event handlers. They need to be declared virtual and
    /// have an implementation that returns false
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool HandleIntentAuxIn( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleNetworkStandbyIntent( )
    {
        return false;
    }

    virtual bool HandleNetworkConfigurationStatus( )
    {
        return false;
    }

    virtual bool HandleBtLeModuleReady( )
    {
        return false;
    }

    static  void SetProductController( ProductController* productController )
    {
        s_productController = productController;
    }
    bool IsProductNeedsSetup();

public:
    /// The custom version of this function returns the custom ProductController
    static EddieProductController& GetCustomProductController();

    static ProductController* s_productController;

    ///Declare pure virtual methods for common event handlers here
    ///and implement them in derived ProductController state class.
};
} //namespace ProductApp
