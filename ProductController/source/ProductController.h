////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductController.h
///
/// @brief     This header file declares a ProductController class that acts as a container to
///            handle all the main functionality related to this program that is not product
///            specific. A single instance of this class is created in the main function for the
///            Product Controller, where the Run method for this instance is called to start and run
///            this program.
///
/// @author    Stuart J. Lumby
///
/// @date      07/15/2017
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareManager;
class ProductDeviceManager;
class ProductUserInterface;
class ProductCommandLine;
class ProductSystemInterface;

enum ProductControllerMessageIdType
{
     AUDIO_LEVEL     = 0,
     AUDIO_TONE      = 1,
     USER_MUTE       = 2,
     SPEAKER_SETTING = 3
};

struct ProductControllerMessage
{
       int         identity;
       std::string name;
       char*       data;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductController
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program that is not product specific, including controlling the product states, as well
///        as to instantiating subclasses to manage the device and lower level hardware, and to
///        interface with the user and higher level applications. Note that only one instantiation
///        of this class is to be created through its GetInstance static method, which returns a
///        single static reference to an instance of this class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductController : public APTask
{
  public:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductController::GetInstance
      ///
      /// @brief  This static method creates the one and only instance of a ProductController
      ///         object. That only one instance is created in a thread safe way is guaranteed by
      ///         the C++ Version 11 compiler.
      ///
      /// @param  void This method does not take any arguments.
      ///
      /// @return This method returns a reference to a ProductController object.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      static ProductController* GetInstance( );

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief  The following public methods are used to start the ProductController instance
      ///         task, set up processing before this task runs, and end the task respectively.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      void Run    ( void );
      void OnEntry( void );
      void End   ( void );

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// The following method is used to handle messaging for the product state machine.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      void HandleMessage( ProductControllerMessage message );

      inline NotifyTargetTaskIF* GetMainTask( )
      {
             return( this );
      }

      inline CThreadMutex& GetLock( )
      {
             return m_lock;
      }

  private:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductController
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
      ProductController( );

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief The following copy constructor and equality operator for this class are private
      ///        and are set to be undefined through the delete keyword. This prevents this class
      ///        from being copied directly, so that only the static method GetInstance to this
      ///        class can be used to get the one sole instance of it.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductController( ProductController const& ) = delete;
      void operator =  ( ProductController const& ) = delete;

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// The following member is a mutual exclusion object used to prevent race conditions.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      CThreadMutex m_lock;

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief The following subclass instances are used to manage the lower level hardware and
      ///        the device, as well as to interface with the user and higher level system
      ///        applications and command line, respectively.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductHardwareManager* m_ProductHardwareManager = nullptr;
      ProductDeviceManager*   m_ProductDeviceManager   = nullptr;
      ProductUserInterface*   m_ProductUserInterface   = nullptr;
      ProductSystemInterface* m_ProductSystemInterface = nullptr;
      ProductCommandLine*     m_ProductCommandLine     = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
