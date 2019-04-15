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
#include "Zone.pb.h"
#include "A4V_RemoteCommunicationService.pb.h"
#include "A4V_RemoteCommClientFactory.h"
#include "A4V_RemoteCommClient.h"
#include "A4V_RemoteCommClientIF.h"
#include "Kepler.pb.h"


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
    using RemoteStatus = RCS_PB_MSG::PairingNotify;
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
    explicit ProductBLERemoteManager( CustomProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );
    void Stop( );

    void Pairing_Start( uint32_t timeout );
    void Pairing_Cancel( );
    void Unpairing_Start( );
    void Unpairing_Cancel( );
    bool IsConnected( );
    void PossiblyPair( );
    void PowerOn( )
    {
        m_poweredOn = true;
        UpdateBacklight( );
    }
    void PowerOff( )
    {
        m_poweredOn = false;
        UpdateBacklight( );
    }

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.../CastleProductControllerCommon/ProductController.cpp:
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*                                 m_ProductTask           = nullptr;
    Callback< ProductMessage >                          m_ProductNotify         = nullptr;
    CustomProductController&                            m_ProductController;
    bool                                                m_remoteConnected       = false;
    bool                                                m_pairingPending        = false;
    RemoteStatus::PairingStatus                         m_remoteStatus          = RemoteStatus::PSTATE_UNKNOWN;
    const int                                           m_PairingTimeout        = 7200;
    bool                                                m_poweredOn             = false;
    bool                                                m_sourceSelectAllowed   = true;
    KeplerPb::KeplerConfig                              m_keplerConfig;
    const KeplerPb::KeplerConfig::StateEntry            m_defaultState;
    const KeplerPb::KeplerConfig::ZoneConfiguration     m_defaultZoneConfig;

    SoundTouchInterface::NowSelectionInfo                                   m_nowSelection;
    SoundTouchInterface::Sources                                            m_sources;
    A4VRemoteCommunication::A4VRemoteCommClientIF::A4VRemoteCommClientPtr   m_RCSClient;

    void UpdateNowSelection( const SoundTouchInterface::NowSelectionInfo& nowSelection );
    void InitializeFrontDoor();
    void InitializeRCS();
    void UpdateAvailableSources( const SoundTouchInterface::Sources& sources );
    void UpdateBacklight( );
    void CheckPairing( );
    static void InitLedsMsg( RCS_PB_MSG::LedsRawMsg_t& leds );
    const std::tuple<const KeplerPb::KeplerConfig::StateEntry&, bool, const KeplerPb::KeplerConfig::ZoneConfiguration> DetermineKeplerState( ) const;
    void GetSourceKeysBacklight( RCS_PB_MSG::LedsRawMsg_t& leds ) const;
    static void SetZone( RCS_PB_MSG::LedsRawMsg_t& leds, int zone, RCS_PB_MSG::LedsRawMsg_t::eLedZoneBits_t state );
    const KeplerPb::KeplerConfig::StateEntry& GetKeplerState( KeplerPb::KeplerConfig::State state ) const;

    static constexpr int            ZONE_FIRST  = 1;
    static constexpr int            ZONE_LAST   = 10;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
