///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductAudioServices.cpp
///
/// @brief     This file contains source code for processing AP audio path events (such as
///            audio volume, muting, source selection, and latency.
///
/// @author    Stuart J. Lumby
///
/// @date      09/17/2017
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SystemUtils.h"
#include "unistd.h"
#include "DPrint.h"
#include "Services.h"
#include "APProductIF.h"
#include "APProductFactory.h"
#include "AudioControls.pb.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "A4VSystemTimeout.pb.h"
#include "A4VPersistence.pb.h"
#include "RebroadcastLatencyMode.pb.h"
#include "NetworkPortDefines.h"
#include "ProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductAudioServices.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef APProductIF::APProductPtr               ProductPointer;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductAudioServices object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///IPCSource_t
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductAudioServices object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductAudioServices* ProductAudioServices::GetInstance( NotifyTargetTaskIF*         mainTask,
                                                         Callback< ProductMessage >  ProductNotify,
                                                         ProductHardwareInterface*   HardwareInterface )
{
    static ProductAudioServices* instance = new ProductAudioServices( mainTask,
                                                                      ProductNotify,
                                                                      HardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Audio Services was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::ProductAudioServices
///
/// @brief  This method is the ProductAudioServices constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductAudioServices::ProductAudioServices( NotifyTargetTaskIF*        mainTask,
                                            Callback< ProductMessage > ProductNotify,
                                            ProductHardwareInterface*  HardwareInterface )

    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::Run
///
/// @brief  This method registers for audio path events.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::Run( )
{
    BOSE_DEBUG( s_logger, "Registration for audio path events is being made." );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback is created that is called whenever an audio selection has occurred.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    m_ProductPointer = APProductFactory::Create( "ProductAudioServices", m_mainTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback is created that is called whenever an audio selection has occurred.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        Callback< uint32_t > callback( std::bind( &ProductAudioServices::SelectCallback,
                                                  this,
                                                  std::placeholders::_1 ) );
        m_ProductPointer->RegisterForSelect( callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback is created that is called whenever an audio selection has occurred.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        Callback< uint32_t > callback( std::bind( &ProductAudioServices::DeselectCallback,
                                                  this,
                                                  std::placeholders::_1 ) );
        m_ProductPointer->RegisterForDeselect( callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback is created that is called whenever volume change occurs.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        Callback< uint32_t > callback( std::bind( &ProductAudioServices::VolumeCallback,
                                                  this,
                                                  std::placeholders::_1 ) );
        m_ProductPointer->RegisterForVolume( callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback is created that is called whenever an internal mute has occurred.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        Callback< bool > callback( std::bind( &ProductAudioServices::UserMuteCallback,
                                              this,
                                              std::placeholders::_1 ) );
        m_ProductPointer->RegisterForUserMute( callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback is created that is called whenever an internal mute has occurred.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        Callback< bool > callback( std::bind( &ProductAudioServices::InternalMuteCallback,
                                              this,
                                              std::placeholders::_1 ) );
        m_ProductPointer->RegisterForInternalMute( callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// A callback for whenever an internal mute has occurred is created.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        Callback< uint32_t > callback( std::bind( &ProductAudioServices::RebroadcastLatencyCallback,
                                                  this,
                                                  std::placeholders::_1 ) );
        m_ProductPointer->RegisterForRebroadcastLatency( callback );
    }

    Callback< bool >callback( std::bind( &ProductAudioServices::ConnectCallback,
                                         this,
                                         std::placeholders::_1 ) );

    m_ProductPointer->Connect( callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::SelectCallback
///
/// @brief  This method passes audio source selection events to the SelectCallbackAction method on
///         the main task.
///
/// @param  source This integer argument identifies the source to be selected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::SelectCallback( uint32_t source )
{
    IL::BreakThread( std::bind( &ProductAudioServices::SelectCallbackAction,
                                this,
                                source ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::SelectCallbackAction
///
/// @brief  This method processes the audio source selection event and sets it on the product.
///
/// @param  source This integer argument identifies the source to be selected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::SelectCallbackAction( uint32_t source )
{
    BOSE_DEBUG( s_logger, "Selection of source %d has been made.", source );

    m_ProductPointer->SetSelectionStatus( APProductIF::SELECTED, source );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::DeselectCallback
///
/// @brief  This method passes audio source deselection events to the SelectCallbackAction method
///         on the main task.
///
/// @param  source This integer argument identifies the source to be deselected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::DeselectCallback( uint32_t source )
{
    IL::BreakThread( std::bind( &ProductAudioServices::DeselectCallbackAction,
                                this,
                                source ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::DeselectCallbackAction
///
/// @brief  This method processes the audio source deselection event and sets it on the product.
///
/// @param  source This integer argument identifies the source to be deselected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::DeselectCallbackAction( uint32_t source )
{
    BOSE_DEBUG( s_logger, "Deselection of source %d has been sent.", source );

    m_ProductPointer->SetSelectionStatus( APProductIF::DESELECTED, source );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::VolumeCallback
///
/// @brief  This method passes audio volume events to the VolumeCallbackAction method on the main
///         task.
///
/// @param  volume This integer argument specifies the volume level.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::VolumeCallback( uint32_t volume )
{
    IL::BreakThread( std::bind( &ProductAudioServices::VolumeCallbackAction,
                                this,
                                volume ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::VolumeCallbackAction
///
/// @brief  This method processes the audio volume event and sets it in the hardware. It is a public
///         method and can also be called through the system interface directly.
///
/// @param  volume This integer argument specifies the volume level.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::VolumeCallbackAction( uint32_t volume )
{
    BOSE_DEBUG( s_logger, "A volume level of %d will be set.", volume );

    m_currentVolume = volume;

    m_ProductHardwareInterface->SendSetVolume( volume );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::UserMuteCallback
///
/// @brief  This method passes user mute events to the UserMuteCallbackAction method on the main
///         task.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::UserMuteCallback( bool mute )
{
    IL::BreakThread( std::bind( &ProductAudioServices::UserMuteCallbackAction,
                                this,
                                mute ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::UserMuteCallbackAction
///
/// @brief  This method process the user mute event and sets it in the hardware. It is a public
///         method and can also be called through the system interface directly.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::UserMuteCallbackAction( bool mute )
{
    BOSE_DEBUG( s_logger, "A user mute %s will be set.", ( mute ? "on" : "off" ) );

    m_userMute = mute;

    m_ProductHardwareInterface->SendUserMute( m_userMute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::InternalMuteCallback
///
/// @brief  This method passes internal mute events to the InternalMuteCallbackAction method on the
///         main task.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::InternalMuteCallback( bool mute )
{
    IL::BreakThread( std::bind( &ProductAudioServices::InternalMuteCallbackAction,
                                this,
                                mute ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::InternalMuteCallbackAction
///
/// @brief  This method process the internal mute event and sets it in the hardware. It is a public
///         method and can also be called through the system interface directly.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::InternalMuteCallbackAction( bool mute )
{
    BOSE_DEBUG( s_logger, "An internal mute %s has been sent.", ( mute ? "on" : "off" ) );

    m_internalMute = mute;

    m_ProductHardwareInterface->SendInternalMute( m_internalMute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::RebroadcastLatencyCallback
///
/// @brief  This method passes rebroadcast latency events to the RebroadcastLatencyCallbackAction
///         method on the main task.
///
/// @param  latency [input] This integer argument specifies the latency value.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::RebroadcastLatencyCallback( uint32_t latency )
{
    IL::BreakThread( std::bind( &ProductAudioServices::RebroadcastLatencyCallbackAction,
                                this,
                                latency ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::RebroadcastLatencyCallbackAction
///
/// @brief  This method processes the rebroadcast latency event and sets it in the hardware. It is
///         a public method and can also be called through the system interface directly.
///
/// @param  latency [input] This integer argument specifies the latency value.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::RebroadcastLatencyCallbackAction( uint32_t latency )
{
    BOSE_DEBUG( s_logger, "A latency value of %d will be set.", latency );

    m_rebroadcastLatency = latency;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// The latency is set based on the argument. Multi-room zone information will need to be
    /// processed in future development.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    m_ProductHardwareInterface->SendAudioPathPresentationLatency( latency );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices::ConnectCallback
///
/// @brief  This method passes a connection event to the ConnectCallbackAction method on
///         the main task.
///
/// @param  connect [input] This Boolean argument identifies the source to be selected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::ConnectCallback( bool connect )
{
    IL::BreakThread( std::bind( &ProductAudioServices::ConnectCallbackAction,
                                this,
                                connect ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioServices`::ConnectCallbackAction
///
/// @brief  This method processes the connection event and sets the volume and mute state in the
///         hardware. It is a public method and can also be called through the system interface
///         directly.
///
/// @param  volume This integer argument specifies the volume level.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAudioServices::ConnectCallbackAction( bool connect )
{
    BOSE_DEBUG( s_logger, "A connection %s event has been sent.", ( connect ? "on" : "off" ) );

    m_ProductPointer->SetVolume( m_currentVolume );
    m_ProductPointer->SetUserMute( m_userMute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
