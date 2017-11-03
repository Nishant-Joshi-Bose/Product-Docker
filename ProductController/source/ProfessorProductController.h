////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProfessorProductController.h
///
/// @brief     This header file declares a ProfessorProductController class that acts as a container
///            to handle all the main functionality related to this program that is not product
///            specific. A single instance of this class is created in the main function for the
///            Product Controller, where the Run method for this instance is called to start and run
///            this program.
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
#include "ThreadMutex.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCMessageRouterIF.h"
#include "ProductNetworkManager.h"
#include "ProductSystemManager.h"
#include "ProductVolumeManager.h"
#include "KeyActions.pb.h"
#include "NetManager.pb.h"
#include "ProductController.h"
#include "ProductControllerHsm.h"
#include "ProductSTSController.h"
#include "Utilities.h"
#include "SoundTouchInterface/PlayerService.pb.h"

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
class ProductAudioService;
class ProductSoftwareServices;
class ProductEdidInterface;
class ProductCommandLine;
class ProductUserInterface;
class ProductSpeakerManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProfessorProductController
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program that is not product specific, including controlling the product states, as well
///        as to instantiating subclasses to manage the device and lower level hardware, and to
///        interface with the user and higher level applications. Note that only one instantiation
///        of this class is to be created through its GetInstance static method, which returns a
///        single static reference to an instance of this class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProfessorProductController : public ProductController
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProfessorProductController::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of a ProfessorProductController
    ///         object. That only one instance is created in a thread safe way is guaranteed by
    ///         the C++ Version 11 compiler.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method returns a reference to a ProfessorProductController object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    static ProfessorProductController* GetInstance( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start the ProfessorProductController
    ///         instance task, set up processing before this task runs, and wait for the task to
    ///         end, and end the task resCustomProductControllerStateBootingpectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( void );
    void OnEntry( void );
    void Wait( void );
    void End( void );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a pointer to the hardware interface instance
    ///        from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface* GetHardwareInterface( void ) const;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a pointer to the volume manager instance
    ///        from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductVolumeManager* GetVolumeManager( void ) const;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a pointer to the speaker manager instance
    ///        from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductSpeakerManager* GetSpeakerManager( void );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBooted( void )                 const;
    bool IsNetworkConfigured( void )      const;
    bool IsNetworkConnected( void )       const;
    bool IsAutoWakeEnabled( void )        const;
    bool IsVoiceConfigured( void )        const;
    bool IsSoftwareUpdateRequired( void ) const;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method selects a new source.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void SelectSource( PlaybackSource_t source );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is called to handle product controller messages, which are
    ///        sent from the more product specific class instances, and is used to process the
    ///        state machine for the product.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void HandleMessage( const ProductMessage& message );

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProfessorProductController
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProfessorProductController( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProfessorProductController( ProfessorProductController const& ) = delete;
    ProfessorProductController operator = ( ProfessorProductController const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following member is used to provide functionality for the product controller state
    /// machine.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductControllerHsm m_ProductControllerStateMachine;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications and command line, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface* m_ProductHardwareInterface;
    ProductSystemManager*     m_ProductSystemManager;
    ProductNetworkManager*    m_ProductNetworkManager;
    ProductAudioService*      m_ProductAudioService;
    ProductSoftwareServices*  m_ProductSoftwareServices;
    ProductCommandLine*       m_ProductCommandLine;
    ProductUserInterface*     m_ProductUserInterface;
    ProductEdidInterface*     m_ProductEdidInterface;
    ProductVolumeManager*     m_ProductVolumeManager;
    ProductSpeakerManager*    m_ProductSpeakerManager;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variables are used to determined whether certain required
    ///        processes or connections are ready for setting the various Product Controller state
    ///        machine states.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
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

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Interfaces to the ProductSTSController, which implements the interactions
    ///        between the Professor Product Controller and the STS source proxies.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductSTSController m_ProductSTSController;

    void SetupProductSTSConntroller( void );
    void HandleSTSInitWasComplete( void );
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );
    void PostPlaybackRequestResponse( const SoundTouchInterface::NowPlayingJson& resp );
    void PostPlaybackRequestError( const FRONT_DOOR_CLIENT_ERRORS errorCode );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
