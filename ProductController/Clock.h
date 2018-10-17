////////////////////////////////////////////////////////////////////////////////
/// @file      Clock.h
///
/// @brief     Clock class.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "CliClientMT.h"
#include "AsyncCallback.h"
#include "DeviceManager.pb.h"
#include "EndPointsError.pb.h"
#include "CommonCLICmdsKeys.h"
#include "ProtoPersistenceFactory.h"
#include "FrontDoorClientIF.h"

namespace ProductApp
{
class ProductController;

class Clock
{
public:
    Clock( std::shared_ptr<FrontDoorClientIF> , NotifyTargetTaskIF*, std::string const& );
    ~Clock();

    ///////////////////////////////////////////////////////////////////////////////
    ///// @name   Initialize
    ///// @brief  Initialize the clock class
    /////////////////////////////////////////////////////////////////////////////////
    void Initialize( );

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   RegisterEndPoints ()
    /// @brief  Register Frontdoor APIs
    ////////////////////////////////////////////////////////////////////////////////
    void RegisterEndPoints();

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   RegisterCliCmds ()
    /// @brief  Register Command Line Interface APIs
    ////////////////////////////////////////////////////////////////////////////////
    void RegisterCliCmds( CliClientMT &cliClient );

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   HandleGetClock ()
    /// @brief  Callback function for GET clock API
    ////////////////////////////////////////////////////////////////////////////////
    void HandleGetClock( Callback<DeviceManagerPb::Clock>,
                         Callback<EndPointsError::Error> );

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   HandlePutClock ()
    /// @brief  Callback function for PUT clock API
    ////////////////////////////////////////////////////////////////////////////////
    void HandlePutClock( DeviceManagerPb::Clock,
                         Callback<DeviceManagerPb::Clock> ,
                         Callback<EndPointsError::Error> );

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   GetClockInfo ()
    /// @brief  Get Clock info
    /// @return clock protobuf
    ////////////////////////////////////////////////////////////////////////////////
    const DeviceManagerPb::Clock &  GetClockInfo() const
    {
        return m_ClockInfo;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   HandleCliCmd ()
    /// @brief  cli command handler.
    ////////////////////////////////////////////////////////////////////////////////
    void HandleCliCmd( uint16_t cmdKey,
                       const std::list<std::string> & argList,
                       AsyncCallback<std::string, int32_t> rspAndRspCmplt,
                       int32_t transact_id );

private:
    ////////////////////////////////////////////////////////////////////////////////
    /// @name   SetClockStatus ()
    /// @brief  set clock enable status.
    ////////////////////////////////////////////////////////////////////////////////
    void SetClockStatus( bool status );

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   LoadFromPersistence ()
    /// @brief  Loading the settings from persistence.
    ////////////////////////////////////////////////////////////////////////////////
    void LoadFromPersistence();

    ////////////////////////////////////////////////////////////////////////////////
    /// @name   SaveToPersistence ()
    /// @brief  Saving the settings to persistence.
    ////////////////////////////////////////////////////////////////////////////////
    void SaveToPersistence();

    std::shared_ptr<FrontDoorClientIF>         m_FrontDoorClientIF;
    NotifyTargetTaskIF*                        m_Task;
    std::string const&                         m_ProductGuid;

    //clock enable info protobuf
    DeviceManagerPb::Clock                     m_ClockInfo;

    //reference to the persistence
    ProtoPersistenceIF::ProtoPersistencePtr    m_ClockPersist = nullptr;


};
} ///namespace ProductApp
