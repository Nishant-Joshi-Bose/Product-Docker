////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProfessorProductController.h
///
/// @brief     This header file declares a ProfessorProductController class that acts as a container
///            to handle all the main functionality related to this program. A single instance of
///            this class is created in the main function for the Product Controller, where the Run
///            method for its instance is called to start and run this program.
///
/// @author    Stuart J. Lumby
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
#include "Utilities.h"
#include "IntentHandler.h"
#include "ProductEdidInterface.h"
#include "ProductController.h"
#include "ProductSTSController.h"
#include "FrontDoorClientIF.h"
#include "ProductMessage.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "MacAddressInfo.h"
#include "BoseVersion.h"

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
class ProductSystemManager;
class ProductNetworkManager;
class CustomProductAudioService;
class ProductEdidInterface;
class ProductCommandLine;
class ProductKeyInputInterface;
class ProductAdaptIQManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProfessorProductController
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program, including controlling the product states, as well as to instantiating subclasses
///        to manage the device and lower level hardware, and to interface with the user and higher
///        level applications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProfessorProductController : public ProductController
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  Constructor for the ProfessorProductController Class
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProfessorProductController( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start the ProfessorProductController
    ///         instance task, wait in a separate task until the product task ends, and stop the
    ///         product task, respectively.
    ///ProductLpmHardwareInterface
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );
    void Wait( );
    void End( );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get the product controller callback function to which
    ///         product message events (based on ProductMessage Protocol Buffers) can be sent.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Callback < ProductMessage > GetMessageHandler( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a reference to the command line interface
    ///        from the common inherited product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    CliClientMT& GetCommandLineInterface( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the LPM hardware interface
    ///        instance from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< CustomProductLpmHardwareInterface >& GetLpmHardwareInterface( ) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the AdaptIQ instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductAdaptIQManager >& GetAdaptIQManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer reference to the Edid
    ///        instance from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductEdidInterface >& GetEdidInterface( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a reference to the intent manager instance for
    ///        processing actions from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    IntentHandler& GetIntentHandler( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBooted( )                 const override;
    bool IsNetworkConfigured( )      const override;
    bool IsNetworkConnected( )       const override;
    bool IsAutoWakeEnabled( )        const override;
    bool IsVoiceConfigured( )        const override;
    bool IsSoftwareUpdateRequired( ) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method selects starts a playback on the specified source.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void SendPlaybackRequest( PlaybackSource_t source );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used to get the currently selected source.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    PlaybackSource_t GetCurrentSource( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used to get the the last Sound Touch playback. It is
    ///        currently is a kludge, until the common code supports persistent storage of this data.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    SoundTouchInterface::playbackRequestJson& GetLastSoundTouchPlayback( );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   GetWiFiOperationalMode
    ///
    /// @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    NetManager::Protobuf::OperationalMode GetWiFiOperationalMode( ) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is called to handle product controller messages, which are
    ///        sent from the more product specific class instances, and is used to process the
    ///        state machine for the product.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleMessage( const ProductMessage& message );

    std::string const& GetDefaultProductName() const override;

    std::string const& GetProductType() const override;

    std::string GetProductColor() const override;

    std::string const& GetProductVariant() const override;

    BLESetupService::ProductId GetProductId() const override
    {
        return BLESetupService::ProductId::PROFESSOR;
    }

    std::string GetProductVersionNumber() const override
    {
        return ( VERSION_STRING_SHORT + std::string( "-" ) + VERSION_BUILD_ABBREV_COMMIT );
    }

    BLESetupService::VariantId GetVariantId() const override
    {
        /// TODO variantID should be from mfg data
        return BLESetupService::VariantId::NONE;
    }

    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    bool IsSystemLanguageSet( ) const;

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclasses declarations are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications and command line.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< CustomProductLpmHardwareInterface > m_ProductLpmHardwareInterface;
    std::shared_ptr< ProductSystemManager              > m_ProductSystemManager;
    std::shared_ptr< ProductNetworkManager             > m_ProductNetworkManager;
    std::shared_ptr< ProductCommandLine                > m_ProductCommandLine;
    std::shared_ptr< ProductKeyInputInterface          > m_ProductKeyInputInterface;
    std::shared_ptr< ProductEdidInterface              > m_ProductEdidInterface;
    std::shared_ptr< ProductAdaptIQManager             > m_ProductAdaptIQManager;
    std::shared_ptr< CustomProductAudioService         > m_ProductAudioService;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variables are used to determined whether certain required
    ///        processes or connections are ready for setting the various Product Controller state
    ///        machine states.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool m_IsLpmReady;
    bool m_IsCapsReady;
    bool m_IsAudioPathReady;
    bool m_IsSTSReady;
    bool m_IsNetworkConfigured;
    bool m_IsNetworkConnected;
    bool m_IsAutoWakeEnabled;
    bool m_IsAccountConfigured;
    bool m_IsMicrophoneEnabled;
    bool m_IsSoftwareUpdateRequired;
    bool m_Running;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are used as interfaces to the ProductSTSController,
    ///        which implements the interactions between the Professor Product Controller and the
    ///        STS source proxies.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductSTSController m_ProductSTSController;

    void SetupProductSTSConntroller( );
    void HandleSTSInitWasComplete( );
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to register for end-points used in the inherited common
    ///        ProductController class, as well as for end-points registered for in this class.
    ///
    /// @todo  These end-points and callbacks may be better placed in a common module, especially
    ///        for end-points being used for playback functionality.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEndPoints( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations is used for internal source selection and playback.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    PlaybackSource_t m_currentSource;
    SoundTouchInterface::playbackRequestJson m_lastSoundTouchPlayback;

    void  SetTestSoundTouchPlayback( );
    void  RegisterNowPlayingEndPoint( );
    void  HandleNowPlaying( const SoundTouchInterface::NowPlayingJson& nowPlayingStatus );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used for intent management based on key actions.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    IntentHandler m_IntentHandler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
