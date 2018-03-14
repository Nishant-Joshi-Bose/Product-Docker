////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSourceInfo.h
///
/// @brief     This file contains the source code to handle communication with A4VVideoManager service
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
#include "BOptional.h"

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

class ProductSourceInfo
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductSourceInfo::ProductSourceInfo
    ///
    /// @param ProductController
    ///
    /// @return This method does not return anything.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ProductSourceInfo( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( void );
    void Stop( void );

    BOptional<SoundTouchInterface::Sources::SourceItem> FindSource( const SoundTouchInterface::ContentItem& item );
    void Refresh( void );

    const SoundTouchInterface::PlaybackRequest& GetGameSourcePlaybackRq()
    {
        return m_gameSourcePlaybackRq;
    }
    const SoundTouchInterface::PlaybackRequest& GetDvdSourcePlaybackRq()
    {
        return m_dvdSourcePlaybackRq;
    }
    const SoundTouchInterface::PlaybackRequest& GetCablesatSourcePlaybackRq()
    {
        return m_cablesatSourcePlaybackRq;
    }

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*             m_ProductTask       = nullptr;
    Callback< ProductMessage >      m_ProductNotify     = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instance is used to communicate with the FrontDoor.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<FrontDoorClientIF>      m_FrontDoorClient;

    SoundTouchInterface::Sources            m_sources;

    //PlaybackRequests associated with user configurable Game, DVD, Cable/Sat activation key on BLE remote
    SoundTouchInterface::PlaybackRequest    m_gameSourcePlaybackRq;
    SoundTouchInterface::PlaybackRequest    m_dvdSourcePlaybackRq;
    SoundTouchInterface::PlaybackRequest    m_cablesatSourcePlaybackRq;

    void UpdateSources( const SoundTouchInterface::Sources& sources );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
