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
#include "ProductEdidInterface.h"
#include "ProductController.h"
#include "ProductSTSController.h"
#include "FrontDoorClientIF.h"
#include "ProductVolumeManager.h"
#include "ProductMessage.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "MacAddressInfo.h"

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
class ProductHardwareInterface;
class ProductSystemManager;
class ProductNetworkManager;
class CustomProductAudioService;
class ProductSoftwareServices;
class ProductEdidInterface;
class ProductCommandLine;
class ProductKeyInputInterface;
class ProductAdaptIQManager;
class ProductSpeakerManager;

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
    ///
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
    /// @brief The following method is used to get a shared pointer to the hardware interface
    ///        instance from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductHardwareInterface >& GetHardwareInterface( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the volume manager instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductVolumeManager >& GetVolumeManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the AdaptIQ instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductAdaptIQManager >& GetAdaptIQManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the speaker manager instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductSpeakerManager >& GetSpeakerManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get get a shared pointer reference to Edid 
    ///        instance from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductEdidInterface >& GetEdidInterface( );


    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBooted( )                 const;
    bool IsNetworkConfigured( )      const;
    bool IsNetworkConnected( )       const;
    bool IsAutoWakeEnabled( )        const;
    bool IsVoiceConfigured( )        const;
    bool IsSoftwareUpdateRequired( ) const;

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
    /// @brief The following method is called to handle product controller messages, which are
    ///        sent from the more product specific class instances, and is used to process the
    ///        state machine for the product.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleMessage( const ProductMessage& message );

    std::string GetDefaultProductName() const override
    {
        /// To-Do: fix the default name
        return "Bose " + MacAddressInfo::GetPrimaryMAC();
    }

    std::string const& GetProductType() const override;

    std::string const& GetProductVariant() const override;

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
    std::shared_ptr< ProductHardwareInterface > m_ProductHardwareInterface;
    std::shared_ptr< ProductSystemManager     > m_ProductSystemManager;
    std::shared_ptr< ProductNetworkManager    > m_ProductNetworkManager;
    std::shared_ptr< ProductSoftwareServices  > m_ProductSoftwareServices;
    std::shared_ptr< ProductCommandLine       > m_ProductCommandLine;
    std::shared_ptr< ProductKeyInputInterface > m_ProductKeyInputInterface;
    std::shared_ptr< ProductEdidInterface     > m_ProductEdidInterface;
    std::shared_ptr< ProductVolumeManager     > m_ProductVolumeManager;
    std::shared_ptr< ProductAdaptIQManager     > m_ProductAdaptIQManager;
    std::shared_ptr< ProductSpeakerManager    > m_ProductSpeakerManager;
    std::shared_ptr< CustomProductAudioService> m_ProductAudioService;

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

    void PostPlaybackRequestResponse( const SoundTouchInterface::NowPlayingJson& resp );
    void PostPlaybackRequestError( const FRONT_DOOR_CLIENT_ERRORS errorCode );
    void RegisterNowPlayingEndPoint( );
    void HandleNowPlaying( const SoundTouchInterface::NowPlayingJson& nowPlayingStatus );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
