////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductAudioServices.h
///
/// @brief     This header file contains declarations for processing audio path events (such as
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
/// The following compiler directive prevents this header file from being included more than
/// once, which will cause multiple declaration compiler errors, in any file that includes it.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ThreadMutex.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "BreakThread.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "IPCMessageRouterIF.h"
#include "IPCDirectoryIF.h"
#include "AudioControls.pb.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "NetworkPortDefines.h"
#include "A4VSystemTimeout.pb.h"
#include "A4VPersistence.pb.h"

namespace ProductApp
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    ///            Forward Class Declarations
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class ProductHardwareInterface;
    class ProductController;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
    ///        inter-thread communications.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef APProductIF::APProductPtr               ProductPointer;
    typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
    typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
    typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
    typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
    typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;

    class ProductAudioServices
    {
      public:

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///
        /// @name   ProductAudioServices::GetInstance
        ///
        /// @brief  This static method creates the one and only instance of a ProductAudioServices
        ///         object. That only one instance is created in a thread safe way is guaranteed by
        ///         the C++ Version 11 compiler.
        ///
        /// @return This method returns a reference to a ProductAudioServices object.
        ///
        //////////////////////////////////////////////////////////////////////////////////////////////
        static ProductAudioServices* GetInstance( NotifyTargetTaskIF*         mainTask,
                                                  Callback< ProductMessage >  ProductNotify,
                                                  ProductHardwareInterface*   HardwareInterface );

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// This declaration is used to start and run audio services.
        //////////////////////////////////////////////////////////////////////////////////////////////
        void Run( void );

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// These declarations are used to handle audio path events on the product.
        //////////////////////////////////////////////////////////////////////////////////////////////
        void ConnectCallbackAction           ( bool          connect );
        void SelectCallbackAction            ( uint32_t      source );
        void DeselectCallbackAction          ( uint32_t      source );
        void VolumeCallbackAction            ( uint32_t      volume );
        void UserMuteCallbackAction          ( bool          mute );
        void InternalMuteCallbackAction      ( bool          mute );
        void RebroadcastLatencyCallbackAction( uint32_t      latency );

    private:

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///
        /// @name   ProductAudioServices
        ///
        /// @brief  The constructor for this class is set to be private. This definition prevents this
        ///         class from being instantiated directly, so that only the static method GetInstance
        ///         to this class can be used to get the one sole instance of it.
        ///
        /// @return This method does not return anything.
        ///
        //////////////////////////////////////////////////////////////////////////////////////////////
        ProductAudioServices( NotifyTargetTaskIF*         mainTask,
                              Callback< ProductMessage >  ProductNotify,
                              ProductHardwareInterface*   HardwareInterface );

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///
        /// @brief The following copy constructor and equality operator for this class are private
        ///        and are set to be undefined through the delete keyword. This prevents this class
        ///        from being copied directly, so that only the static method GetInstance to this
        ///        class can be used to get the one sole instance of it.
        ///
        //////////////////////////////////////////////////////////////////////////////////////////////
        ProductAudioServices( ProductAudioServices const& ) = delete;
        void operator   =   ( ProductAudioServices const& ) = delete;

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// This declaration stores the main task for processing audio path services. It is
        /// inherited by the ProductController instance.
        //////////////////////////////////////////////////////////////////////////////////////////////
        NotifyTargetTaskIF*        m_mainTask                 = nullptr;
        Callback< ProductMessage > m_ProductNotify            = nullptr;
        ProductHardwareInterface*  m_ProductHardwareInterface = nullptr;

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// These declarations are used for handling product audio events.
        //////////////////////////////////////////////////////////////////////////////////////////////
        ProductPointer m_ProductPointer = nullptr;

        void ConnectCallback           ( bool          connect );
        void RegisterForProductEvents  ( void );
        void SelectCallback            ( uint32_t      source );
        void DeselectCallback          ( uint32_t      source );
        void VolumeCallback            ( uint32_t      volume );
        void UserMuteCallback          ( bool          mute );
        void InternalMuteCallback      ( bool          mute );
        void RebroadcastLatencyCallback( uint32_t      latency );

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// The following declarations are for handling audio path specific requests.
        //////////////////////////////////////////////////////////////////////////////////////////////
        uint32_t m_currentVolume      = 0;
        uint32_t m_rebroadcastLatency = 0;
        bool     m_userMute           = true;
        bool     m_internalMute       = true;
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
