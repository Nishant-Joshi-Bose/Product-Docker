////////////////////////////////////////////////////////////////////////////////
/// @file   FrontDoorClient.h
/// @brief  Class for connection management to web socket server (front end).
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SoundTouchClientSocket.h"
#include "SoundTouchInterface/Msg.pb.h"
#include "APTask.h"

class NotifyTargetTaskIF;

namespace ProductApp
{

using google::protobuf::Message;

class FrontDoorClient: public SoundTouch::ClientSocket, public APTask
{
public:
    FrontDoorClient(std::string host );
    ~FrontDoorClient();

    FrontDoorClient( const FrontDoorClient& ) = delete;
    FrontDoorClient& operator=( const FrontDoorClient& ) = delete;


    void OnOpen() override;
    void OnClose() override;
    void OnMessage( std::string ) override;

    using ClientSocket::Send;

protected:

    /*!
     * Convert a message to JSON/XML and then write it to the socket.
     *
     * The message is actually queued to be sent to the server, not
     * necessarily sent immediately.  See ClientSocket::Send for more
     * information.
     *
     * @param msg The message to send.
     *
     * @return true if the message is queued, false if the queue is full.
     */
    bool Send( Message const& msg );

private:
    void OnEntry() override;

    NotifyTargetTaskIF* m_task = nullptr;
};
}; // namespace ProductApp 
