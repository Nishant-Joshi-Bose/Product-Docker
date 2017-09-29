////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDeviceSettings.h
///
/// @brief     This header file contains declarations for managing the device settings, which
///            includes requests for DSP audio, speaker settings, and product HDMI control.
///
/// @author    Stuart J. Lumby
///
/// @date      09/22/2017
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
#include "WebExtensionsUrlHandler.h"
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

    class ProductDeviceSettings
    {
      public:

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///
        /// @name   ProductDeviceSettings::GetInstance
        ///
        /// @brief  This static method creates the one and only instance of a ProductDeviceSettings
        ///         object. That only one instance is created in a thread safe way is guaranteed by
        ///         the C++ Version 11 compiler.
        ///
        /// @return This method returns a reference to a ProductDeviceSettings object.
        ///
        //////////////////////////////////////////////////////////////////////////////////////////////
        static ProductDeviceSettings* GetInstance( NotifyTargetTaskIF*         mainTask,
                                                   Callback< ProductMessage >  ProductNotify,
                                                   ProductHardwareInterface*   HardwareInterface );

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// This declaration is used to start and run the device settings.
        //////////////////////////////////////////////////////////////////////////////////////////////
        void Run( void );

      private:

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///
        /// @name   ProductDeviceSettings
        ///
        /// @brief  The constructor for this class is set to be private. This definition prevents this
        ///         class from being instantiated directly, so that only the static method GetInstance
        ///         to this class can be used to get the one sole instance of it.
        ///
        /// @return This method does not return anything.
        ///
        //////////////////////////////////////////////////////////////////////////////////////////////
        ProductDeviceSettings( NotifyTargetTaskIF*         mainTask,
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
        ProductDeviceSettings( ProductDeviceSettings const& ) = delete;
        void operator   =    ( ProductDeviceSettings const& ) = delete;

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// This declaration stores the main task for processing device events and requests. It is
        /// inherited by the ProductController instance.
        //////////////////////////////////////////////////////////////////////////////////////////////
        NotifyTargetTaskIF*        m_mainTask                 = nullptr;
        Callback< ProductMessage > m_ProductNotify            = nullptr;
        ProductHardwareInterface*  m_ProductHardwareInterface = nullptr;

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// These declarations are used for handling product audio events.
        //////////////////////////////////////////////////////////////////////////////////////////////
        audiodspcontrols                m_audioDSPControlStore;
        audioproducttonecontrols        m_audioProductToneControlStore;
        audioproductlevelcontrols       m_audioProductSpeakerLevelStore;
        audiospeakerattributeandsetting m_audioSpeakerAttributeAndSettingStore;
        systemtimeoutcontrol            m_systemTimeoutControlStore;
        rebroadcastlatencymode          m_rebroadcastLatencyModeStore;

        WebExtensionsUrlHandler< audiodspcontrols                >* m_audioDSPControlHandler                 = nullptr;
        WebExtensionsUrlHandler< audioproducttonecontrols        >* m_audioProductToneControlHandler         = nullptr;
        WebExtensionsUrlHandler< audioproductlevelcontrols       >* m_audioProductSpeakerLevelHandler        = nullptr;
        WebExtensionsUrlHandler< audiospeakerattributeandsetting >* m_audioSpeakerAttributeAndSettingHandler = nullptr;
        WebExtensionsUrlHandler< systemtimeoutcontrol            >* m_systemTimeoutControlHandler            = nullptr;
        WebExtensionsUrlHandler< rebroadcastlatencymode          >* m_rebroadcastLatencyModeHandler          = nullptr;

        audiodspcontrols& GetAudioDSPControlStore( )
        {
            return m_audioDSPControlStore;
        }

        audioproducttonecontrols& GetAudioToneControlStore( )
        {
            return m_audioProductToneControlStore;
        }

        audioproductlevelcontrols& GetAudioSpeakerLevelStore( )
        {
            return m_audioProductSpeakerLevelStore;
        }

        audiospeakerattributeandsetting& GetSpeakerAttributeAndSettingStore( )
        {
            return m_audioSpeakerAttributeAndSettingStore;
        }

        systemtimeoutcontrol& GetSystemTimeoutEnableBits( )
        {
            return m_systemTimeoutControlStore;
        }

        rebroadcastlatencymode& GetRebroadcastLatencyModeStore( )
        {
            return m_rebroadcastLatencyModeStore;
        }

        bool SanitizeAudioDSPControls( audiodspcontrols&                          protobuf );
        bool SanitizeAudioToneControls( audioproducttonecontrols&                 protobuf ) const;
        bool SanitizeAudioLevelControls( audioproductlevelcontrols&               protobuf ) const;
        bool SanitizeSpeakerAttributeAndSetting( audiospeakerattributeandsetting& protobuf );
        bool SanitizeRebroadcastLatencyMode( rebroadcastlatencymode&              protobuf );

        void SendDSPAudioControls( void );
        void SendToneAndLevelControls( void );
        void SendSpeakerSettings( void );
        void SendSystemTimeoutEnableBits( void );
        void SendRebroadcastLatencyMode( void );
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
