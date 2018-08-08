////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductCecHelper.h
///
/// @brief     This header file contains declarations to handle communication with a A4VVideoManager
///            service.
///
/// @author    Manoranjani Malisetti
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

///////////////////////////////////////////////////////////////////////////////////////////////////
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
#include "A4VVideoManagerClientFactory.h"
#include "ProductFrontDoorUtility.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "SoundTouchInterface/AudioService.pb.h"
#include "ProductMessage.pb.h"
#include "CecMode.pb.h"
#include "DataCollectionClientIF.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProtoPersistenceFactory.h"
#include "ProtoPersistenceIF.h"

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
class CustomProductLpmHardwareInterface;
class CustomProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductCecHelper
///
/// @brief This class is used to handle communication with a A4VVideoManager service.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductCecHelper
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductCecHelper Constructor
    ///
    /// @param  CustomProductController& ProductController
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductCecHelper( CustomProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following public methods are used to run and stop instances of the
    ///        ProductCecHelper class, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Run( );
    void Stop( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// The following methods are used to inform the video manager that either power has been
    /// enabled or that it should prepare for power to be disabled
    //////////////////////////////////////////////////////////////////////////////////////////////
    void PowerOff( );
    void PowerOn( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method populates the default values in the "properties" field of an
    /// CecMode message.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void SetCecModeDefaultProperties( ProductPb::CecModeResponse& );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method propagates the volume value
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void HandleFrontDoorVolume( SoundTouchInterface::volume const& volume );

private:

    bool                    m_LpmPowerIsOn              = false;
    LPM_IPC_SOURCE_ID       m_LpmSourceID               = LPM_IPC_INVALID_SOURCE;
    bool                    m_HavePhysicalAddress       = false;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are used to interface with the product controller and
    ///        the lower level LPM hardware, as well as the A4VVideoManager service.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_ProductTask;
    Callback< ProductMessage > m_ProductNotify;
    std::shared_ptr < CustomProductLpmHardwareInterface > m_ProductLpmHardwareInterface;
    A4VVideoManager::A4VVideoManagerClientIF::A4VVideoManagerClientPtr m_CecHelper;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This holds the cached status for the CecMode response.
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductPb::CecModeResponse        m_cecresp;

    ////////////////////////////////////////////////////////////////////////////////////////
    /// Persistence interface instance
    //////////////////////////////////////////////////////////////////////////////////////
    ProtoPersistenceIF::ProtoPersistencePtr     m_cecModePersistence = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This member determines whether a connections to the LPM server connection is
    ///        established.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool m_connected = false;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable stores the custom product controller instance.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    CustomProductController& m_CustomProductController;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declares a client for handling data collection.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< DataCollectionClientIF > m_DataCollectionClient;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is called when an A4VVM server connection is established.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Connected( bool  connected );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to register for and receive key events from the
    ///        A4VVideoManager interface.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void HandleHpdEvent( A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent );
    void HandleRawEDIDResponse( const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid );
    void HandlePhyAddrResponse( const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t cecPhysicalAddress );
    void HandleNowPlaying( const SoundTouchInterface::NowPlaying& nowPlayingStatus );
    void HandleSrcSwitch( const LpmServiceMessages::IPCSource_t cecSource );
    void HandlePlaybackRequestResponse( const SoundTouchInterface::NowPlaying& response );
    void HandlePlaybackRequestError( const FrontDoor::Error& error );
    std::shared_ptr< FrontDoorClientIF >    m_FrontDoorClient;
    void CecModeHandleGet( const Callback<const ProductPb::CecModeResponse> & resp, const Callback<FrontDoor::Error> & errorRsp );
    void CecModeHandlePut( const ProductPb::CecUpdateRequest req, const Callback<const ProductPb::CecModeResponse> & resp, const Callback<FrontDoor::Error> & errorRsp );
    void PerhapsSetCecSource( );
    void SendCecSourceSelection( LPM_IPC_SOURCE_ID source );
    void HandleCecState( const IpcCecState_t& state );
    void PersistCecMode( void );
    void LoadFromPersistence( void );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following members store instances of the FrontDoor callbacks.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    CallbackConnection      m_PutConnection;
    CallbackConnection      m_GetConnection;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This holds the last-received CEC state from the LPM
    //////////////////////////////////////////////////////////////////////////////////////////////
    IpcCecState_t           m_cecState;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
