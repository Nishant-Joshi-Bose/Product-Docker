////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductBLERemoteManager.h
///
/// @brief     This file contains the source code to handle communication with A4VRemoteCommunicationService service
///
/// @author    Chris Houston
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
#include "NotifyTargetTaskIF.h"
#include "FrontDoorClient.h"
#include "ContentSelectionService.pb.h"
#include "A4V_RemoteCommunicationService.pb.h"
#include "A4V_RemoteCommClientFactory.h"
#include "A4V_RemoteCommClient.h"
#include "A4V_RemoteCommClientIF.h"
#include "APTimer.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductLpmHardwareInterface;
class ProductController;

class ProductBLERemoteManager
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductBLERemoteManager::ProductBLERemoteManager
    ///
    /// @param ProductController
    ///
    /// @return This method does not return anything.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    explicit ProductBLERemoteManager( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( void );
    void Stop( void );

    void Pairing_Start( uint32_t timeout );
    void Pairing_Cancel( void );
    void Unpairing_Start( void );
    void Unpairing_Cancel( void );
    bool IsConnected( void );

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.../CastleProductControllerCommon/ProductController.cpp:
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*             m_ProductTask       = nullptr;
    Callback< ProductMessage >      m_ProductNotify     = nullptr;
    ProfessorProductController&     m_ProductController;
    APTimerPtr                      m_statusTimer;
    bool                            m_remoteConnected   = false;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instance is used to communicate with the FrontDoor.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<FrontDoorClientIF>                                      m_FrontDoorClient;
    SoundTouchInterface::NowSelectionInfo                                   m_nowSelection;
    SoundTouchInterface::Sources                                            m_sources;
    A4VRemoteCommunication::A4VRemoteCommClientIF::A4VRemoteCommClientPtr   m_RCSClient;

    void UpdateNowSelection( const SoundTouchInterface::NowSelectionInfo& nowSelection );
    void InitializeFrontDoor();
    void InitializeRCS();
    void UpdateAvailableSources( const SoundTouchInterface::Sources& sources );
    void UpdateBacklight( );
    bool GetSourceLED( A4VRemoteCommunication::A4VRemoteCommClientIF::ledSourceType_t& sourceLED );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
