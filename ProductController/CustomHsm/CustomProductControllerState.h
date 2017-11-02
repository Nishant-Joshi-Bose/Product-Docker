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
#include "ProductController.h"

namespace ProductApp
{
class EddieProductController;
class ProductControllerHsm;

class CustomProductControllerState : public CHsmState
{
public:
    CustomProductControllerState( ProductControllerHsm& hsm, CHsmState* pSuperState,
                                  Hsm::STATE stateId = 0, const std::string& name = "Top" );

    virtual ~CustomProductControllerState()
    {
    }

    void HandleStateEnter() override
    {
    }

    void HandleStateStart() override
    {
    }

    void HandleStateExit() override
    {
    }

    virtual bool HandleLpmState( bool isActive )
    {
        return false;
    }

    virtual bool HandleLpmInterfaceState( bool isConnected )
    {
        return false;
    }

    virtual bool HandleModulesReady( )
    {
        return false;
    }

    virtual bool HandleIntents( KeyHandlerUtil::ActionType_t result )
    {
        return false;
    }

    virtual bool HandleNetworkConfigurationStatus( const NetManager::Protobuf::NetworkStatus & networkStatus, int profileSize )
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
