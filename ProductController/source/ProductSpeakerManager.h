////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSpeakerManager.h
///
/// @brief     This header file contains declarations for managing the wireless accessories,
///            including pairing and active speaker control.
///
/// @author    Derek Richardson
///
/// @date      09/22/2017
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

#pragma once

#include <memory>
#include "Callback.h"
#include "LpmClientIF.h"
#include "APProductIF.h"
#include "AsyncCallback.h"
#include "ProductMessage.pb.h"
#include "FrontDoorClientIF.h"
#include "NotifyTargetTaskIF.h"
#include "ProductHardwareInterface.h"
#include "ProductNetworkManager.h"
#include "ProductSpeakerManager.pb.h"

namespace ProductApp
{

class ProductSpeakerManager
{

public:
    static ProductSpeakerManager* GetInstance( NotifyTargetTaskIF* mainTask,
                                               Callback< ProductMessage > ProductNotify,
                                               ProductHardwareInterface*  hardwareInterface );
    bool Run( );
    void DoPairing( );
    void StopPairing( );

private:
    NotifyTargetTaskIF*                m_mainTask          = nullptr;
    Callback< ProductMessage >         m_ProductNotify     = nullptr;
    ProductHardwareInterface*          m_hardwareInterface = nullptr;
    std::shared_ptr<FrontDoorClientIF> m_FrontDoorClientIF = nullptr;
    ProductPb::AccessorySpeakerState   m_accessorySpeakerState;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Handle lpm connection info
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    bool m_lpmConnected;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Misc internal
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void Init();
    ProductSpeakerManager( NotifyTargetTaskIF* task,
                           Callback< ProductMessage > ProductNotify,
                           ProductHardwareInterface*  hardwareInterface );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Functions to register for events
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvents();
    void RegisterLpmClientEvents();
    void SetLpmConnectionState( bool connected );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /// Functions to internally interact with LPM speaker code
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void PairingFrontDoorRequestCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB, LpmServiceMessages::IpcSpeakerPairingMode_t pair );
    void PairingCallback( LpmServiceMessages::IpcSpeakerPairingMode_t pair );
    void DoPairingFrontDoor( bool pair, const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB );

    void DisbandAccessories( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB );

    void SetSpeakersEnabledCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB, const LpmServiceMessages::IpcSpeakersActive_t req );
    void SetSpeakersEnabled( const ProductPb::AccessorySpeakerState::SpeakerControls req, const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB );

    void RecieveAccessoryListCallback( LpmServiceMessages::IpcAccessoryList_t accList );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handler etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackConnection m_registerGetAccessoriesCb;
    CallbackConnection m_registerPutAccessoriesCb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor callback functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void AccessoriesGetHandler( const Callback<ProductPb::AccessorySpeakerState> &resp );
    void AccessoriesPutHandler( const ProductPb::AccessorySpeakerState &req, const Callback<ProductPb::AccessorySpeakerState> &resp );


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Stuff to convert IpcAccessoryList_t to AccessorySpeakerState
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool AccessoryStatusIsConnected( unsigned int status );
    static bool AccessoryTypeIsRear( unsigned int type );
    static bool AccessoryTypeIsSub( unsigned int type );
    static const char* AccessoryTypeToString( unsigned int type );
    static void AccessoryDescriptionToAccessorySpeakerInfo( const LpmServiceMessages::AccessoryDescription_t &accDesc,
                                                            ProductPb::AccessorySpeakerState::AccessorySpeakerInfo* spkrInfo );
};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////

