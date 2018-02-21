////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductStart.cpp
///
/// @brief     This source code file contains the main function used to start and run a Product
///            Controller process on a Linux operating system for Bose Professor Audio for Video
///            speaker products. In the main function, an instance of a ProfessorProductController
///            class is created. This class instance is used as a container to control the product
///            states, as well as to instantiate modules to manage the device and lower level
///            hardware, as well as interface with the user and system level applications.
///            Additionally, this source code file is used to handle any arguments passed to the
///            command used to execute the Product Controller, and to handle any actions required
///            when the associated process crashes through a segmentation fault or receives a
///            signal to terminate.
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SystemUtils.h"
#include "Utilities.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Global Product logger instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{
DPrint s_logger __attribute__( ( init_priority( 101 ) ) ) { "ProfessorProduct" } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProcessShutDown
///
/// @brief  This function is called whenever a termination signal by the system or by the user
///         (typically through a Ctrl+c key press) is received. Any actions that need to be taken
///         when the product controller shuts down should be handled from this function.
///
/// @param  int signal This integer argument stores the type of Linux signal sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessShutDown( int signal )
{
    if( signal == SIGTERM )
    {
        exit( EXIT_SUCCESS );
    }
    else if( signal == SIGINT )
    {
        exit( EXIT_SUCCESS );
    }
    else
    {
        exit( EXIT_FAILURE );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ShowBacktrace
///
/// @brief  This function attempts to print out the stack after a memory violation segmentation
///         fault before the process crashes. Note that although this may be useful for quickly
///         assessing these faults, a solid gdb debugging tool also exists for this purpose.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ShowBacktrace( int sig )
{
    ///
    /// Attempt to print out the stack, resume, and allow the process to die.
    ///
    signal( SIGSEGV, SIG_DFL );

    BOSE_ERROR( ProductApp::s_logger, "------- Product Controller Segmentation Fault Start --------" );

    for( auto function : backtrace( ) )
    {
        BOSE_ERROR( ProductApp::s_logger, function.length( ) ? function.c_str() : "<unresolved>" );
    }

    BOSE_ERROR( ProductApp::s_logger, "-------- Product Controller Segmentation Fault End ---------" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   main
///
/// @brief  This function is the standard C++ main function called when the Product Controller
///         process first starts. It first assigns functions to handle any actions required
///         when the associated process receives a signal to terminate as well as when a
///         segmentation fault occurs, and sets the process to ignore any SIGPIPE signals that
///         indicate that a failed write request took place, which would normally cause the process
///         to terminate. It then makes a system utility call to ensure that only one instance of
///         this process is running. Lastly, it creates a single instance of a
///         ProfessorProductController class to start and run the process. Note that any exceptions
///         generated by the Product Controller process that are not caught in lower level functions
///         or methods should be caught in this main function.
///
/// @param  int argumentCount     This integer argument stores the number of parameters passed to
///                               the command used to execute the Product Controller process. This
///                               number counts the name of the program itself as a parameter.
///
/// @param  char** argumentValue  This argument is a pointer to an array of strings (specified as
///                               character pointers) that store in order the names of the
///                               parameters passed to the command used to execute the Product
///                               Controller process. The name of the program is included as the
///                               first string in the string array.
///
/// @return This method returns an integer to indicate whether the associated Product Controller
///         process ended successfully or not.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argumentCount, char** argumentValue )
{
    try
    {
        signal( SIGTERM, ProcessShutDown );
        signal( SIGINT,  ProcessShutDown );
        signal( SIGSEGV, ShowBacktrace );
        signal( SIGPIPE, SIG_IGN );

        SystemUtils::ThereCanBeOnlyOne( );

        BOSE_DEBUG( ProductApp::s_logger, "----------------- Product Controller Start -----------------" );
        BOSE_DEBUG( ProductApp::s_logger, "The Product Controller is starting up from the main function." );

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief A Professor Product Controller is constructed and ran, and then the main task is
        ///        suspended until the task associated with the Product Controller is ended.
        ////////////////////////////////////////////////////////////////////////////////////////////
        ProductApp::ProfessorProductController ProfessorProductControllerInstance;

        ProfessorProductControllerInstance.Run( );
        ProfessorProductControllerInstance.Wait( );

        return( EXIT_SUCCESS );
    }
    catch( std::exception const& error )
    {
        BOSE_ERROR( ProductApp::s_logger, "------------------ Product Controller End ------------------" );
        BOSE_ERROR( ProductApp::s_logger, "The Product Controller is shutting down." );
        BOSE_ERROR( ProductApp::s_logger, "An exception %s was caught in the attempt.", error.what( ) );

        return( EXIT_FAILURE );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
