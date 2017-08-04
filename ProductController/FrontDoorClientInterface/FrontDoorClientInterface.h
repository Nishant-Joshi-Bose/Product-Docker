////////////////////////////////////////////////////////////////////////////////
/// @file   FrontDoorClientInterface.h
/// @brief  Interface class to frontdoor client.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SoundTouchClient.h"
#include "SoundTouchInterface/Msg.pb.h"
#include "APTask.h"

class NotifyTargetTaskIF;

namespace ProductApp
{
using EndPointRequestCallback = Callback<SoundTouchInterface::msg_Header const&, std::string const&, std::string const&>;

using google::protobuf::Message;

class FrontDoorClientInterface: public SoundTouch::Client, public APTask
{
public:
    FrontDoorClientInterface( const std::string productName, std::string host, NotifyTargetTaskIF* pTask );
    ~FrontDoorClientInterface();

    FrontDoorClientInterface( const FrontDoorClientInterface& ) = delete;
    FrontDoorClientInterface& operator=( const FrontDoorClientInterface& ) = delete;


    void OnOpen() override;
    void OnClose() override;
    /// Request/Response message from FrontDoor
    void OnMessage( SoundTouchInterface::activateProductRequest req, SoundTouchInterface::msg msg ) override;
    void OnMessage( SoundTouchInterface::initializationCompleteRequest req, SoundTouchInterface::msg msg ) override;
    bool UnhandledRequest( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation ) override;

public:
    void RegisterEndPoint( std::string, const EndPointRequestCallback cb );

private:
    void OnEntry() override;
    void FrontDoorClientRun();

    NotifyTargetTaskIF* m_ProductControllerTask = nullptr;
    bool m_Running = true;
    std::map<std::string, EndPointRequestCallback> m_EndPointRequestCallbackMap;
};
} // namespace ProductApp
