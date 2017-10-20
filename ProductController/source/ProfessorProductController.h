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
#include "ProductMessage.pb.h"
#include "ProductNetworkManager.h"
#include "ProductSystemManager.h"
#include "KeyActions.h"
#include "NetManager.pb.h"
#include "ProductController.h"
#include "ProductControllerHsm.h"
#include "ProductControllerStateTop.h"
#include "ProductControllerStateSetup.h"
#include "ProductControllerStates.h"
#include "ProductSTSController.h"
#include "CustomProductControllerState.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateUpdatingSoftware.h"
#include "CustomProductControllerStateOff.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlayable.h"
#include "CustomProductControllerStateNetworkStandby.h"
#include "CustomProductControllerStateNetworkStandbyConfigured.h"
#include "CustomProductControllerStateNetworkStandbyUnconfigured.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateIdleVoiceConfigured.h"
#include "CustomProductControllerStateIdleVoiceUnconfigured.h"
#include "CustomProductControllerStatePlayingActive.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingInactive.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
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
class ProductCommandLine;
class ProductUserInterface;
class CustomProductControllerStateBooting;
class CustomProductControllerStateUpdatingSoftware;
class CustomProductControllerStateOff;
class CustomProductControllerStateOn;
class CustomProductControllerStatePlayable;
class CustomProductControllerStateNetworkStandby;
class CustomProductControllerStateNetworkStandbyConfigured;
class CustomProductControllerStateNetworkStandbyUnconfigured;
class CustomProductControllerStateIdle;
class CustomProductControllerStateIdleVoiceConfigured;
class CustomProductControllerStateIdleVoiceUnconfigured;
class CustomProductControllerStatePlaying;
class CustomProductControllerStatePlayingActive;
class CustomProductControllerStatePlayingInactive;


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

    ProductHardwareInterface* GetHardwareInterface( void );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBooted( void );
    bool IsNetworkConfigured( void );
    bool IsNetworkConnected( void );
    bool IsAutoWakeEnabled( void );
    bool IsVoiceConfigured( void );
    bool IsSoftwareUpdateRequired( void );

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
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method does not return anything.
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
    /// The following members are used to provide functionality for a common product controller
    /// state machine, along with custom states for the Professor platform. The source code and
    /// associated header files to include are found in the CastleProductControllerCommon
    /// Repository, which is imported as a library when this application is built.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductControllerHsm        m_ProductControllerStateMachine;
    ProductControllerStateTop   m_ProductControllerStateTop;
    ProductControllerStateSetup m_ProductControllerStateSetup;

    CustomProductControllerStateBooting                    m_CustomProductControllerStateBooting;
    CustomProductControllerStateUpdatingSoftware           m_CustomProductControllerStateUpdatingSoftware;
    CustomProductControllerStateOff                        m_CustomProductControllerStateOff;
    CustomProductControllerStateOn                         m_CustomProductControllerStateOn;
    CustomProductControllerStatePlayable                   m_CustomProductControllerStatePlayable;
    CustomProductControllerStateNetworkStandby             m_CustomProductControllerStateNetworkStandby;
    CustomProductControllerStateNetworkStandbyConfigured   m_CustomProductControllerStateNetworkStandbyConfigured;
    CustomProductControllerStateNetworkStandbyUnconfigured m_CustomProductControllerStateNetworkStandbyUnconfigured;
    CustomProductControllerStateIdle                       m_CustomProductControllerStateIdle;
    CustomProductControllerStateIdleVoiceConfigured        m_CustomProductControllerStateIdleVoiceConfigured;
    CustomProductControllerStateIdleVoiceUnconfigured      m_CustomProductControllerStateIdleVoiceUnconfigured;
    CustomProductControllerStatePlaying                    m_CustomProductControllerStatePlaying;
    CustomProductControllerStatePlayingActive              m_CustomProductControllerStatePlayingActive;
    CustomProductControllerStatePlayingInactive            m_CustomProductControllerStatePlayingInactive;

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
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
