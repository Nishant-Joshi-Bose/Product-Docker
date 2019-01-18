////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      SpeakerPairingManager.h
///
/// @brief     This header file declares an intent manager class for managing the wireless
///            speaker accessories, including pairing and active speaker control.
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
#include "Utilities.h"
#include "FrontDoorClientIF.h"
#include "LpmServiceMessages.pb.h"
#include "ProductSpeakerManager.pb.h"
#include "ProductMessage.pb.h"
#include "SharedProto.pb.h"
#include "DataCollectionClientIF.h"

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
/// @brief The SpeakerPairingManager Class
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class SpeakerPairingManager: public IntentManager
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Constructor for the SpeakerPairingManager Class
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
    SpeakerPairingManager( NotifyTargetTaskIF&        task,
                           const CliClientMT&         commandLineClient,
                           const FrontDoorClientIF_t& frontDoorClient,
                           ProductController&         productController );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Destructor for the SpeakerPairingManager Class
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ~SpeakerPairingManager( ) override
    {

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name  Handle
    ///
    /// @brief This method is used to handle speaker pairing action intents.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool Handle( KeyHandlerUtil::ActionType_t& action ) override;

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start, and stop pairing for instances
    ///         this class, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void DoPairing( );
    void StopPairing( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable stores the custom product controller instance.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    CustomProductController& m_CustomProductController;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are used to interface with the product controller and
    ///        the lower level LPM hardware.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*                                  m_ProductTask;
    std::shared_ptr< CustomProductLpmHardwareInterface > m_ProductLpmHardwareInterface;
    std::shared_ptr<FrontDoorClientIF>                   m_FrontDoorClientIF;
    std::shared_ptr< DataCollectionClientIF >            m_DataCollectionClient;

    ProductPb::AccessorySpeakerState                     m_accessorySpeakerState;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method performs needed initialization before running.
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize( );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following Boolean is used to handle LPM connection info.
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    bool m_lpmConnected;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used to register for events.
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvents( );
    void RegisterLpmClientEvents( );
    void SetLpmConnectionState( bool connected );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following are methods to internally interact with LPM speaker code.
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void PairingFrontDoorRequestCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB,
                                          LpmServiceMessages::IpcSpeakerPairingMode_t pair );
    void PairingCallback( LpmServiceMessages::IpcSpeakerPairingMode_t pair );
    void DoPairingFrontDoor( bool pair,
                             const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB );

    void DisbandAccessories( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB );

    void DisbandAccessoriesCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB,
                                     LpmServiceMessages::IpcAccessoryDisbandCommand_t accDisband );

    void SetSpeakersEnabledCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB,
                                     const LpmServiceMessages::IpcSpeakersActive_t req );
    void SetSpeakersEnabled( const ProductPb::AccessorySpeakerState::SpeakerControls req,
                             const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB );

    void ReceiveAccessoryListCallback( LpmServiceMessages::IpcAccessoryList_t accList );

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method sends accessory pairing state ProductMessage to ProductController
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void SendAccessoryPairingStateToProduct( );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration are used as Front Door handlers.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    CallbackConnection m_registerGetAccessoriesCb;
    CallbackConnection m_registerPutAccessoriesCb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used as Front Door callbacks.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void AccessoriesGetHandler( Callback<ProductPb::AccessorySpeakerState> resp,
                                Callback<FrontDoor::Error> error );
    void AccessoriesPutHandler( ProductPb::AccessorySpeakerState req,
                                Callback<ProductPb::AccessorySpeakerState> resp,
                                Callback<FrontDoor::Error> error );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are utility methods for determining the accessories status and
    ///        types.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* AccessoryRearConiguration( uint8_t numLeft, uint8_t numRight, uint8_t oldSize );
    bool AccessoryStatusIsConnected( unsigned int status );
    bool AccessoryTypeIsRear( unsigned int type );
    bool AccessoryTypeIsSub( unsigned int type );
    const char* AccessoryTypeToString( unsigned int type );
    void AccessoryDescriptionToAccessorySpeakerInfo( const LpmServiceMessages::AccessoryDescription_t&
                                                     accDesc,
                                                     ProductPb::AccessorySpeakerState::AccessorySpeakerInfo*
                                                     spkrInfo );

    void DetectMissingSub( const ProductPb::AccessorySpeakerState& oldAccessorySpeakerState );
    void DetectMissingRears( const ProductPb::AccessorySpeakerState& oldAccessorySpeakerState );
    void RearAccessoryConnectTimeout();

private:
    APTimerPtr  m_timerRearAccessoryConnect;
    bool        m_accessoryListReceived = false;
    bool        m_firstAccessoryListReceived = false;
    bool        m_waitRearAccessoryConnect = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
