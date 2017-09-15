////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductStart.cpp
///
/// @brief     This source code file contains the main function used to start and run a Product
///            Controller process on a Linux operating system for various Bose speaker products. In
///            the main function, a single instance of a ProductController class is obtained. This
///            class instance is used as a container to control the product states, as well as to
///            instantiate subclasses to manage the device and lower level hardware, and interface
///            with the user and system level applications. Additionally, this source code file is
///            used to handle any arguments passed to the command used to execute the Product
///            Controller, and to handle any actions required when the associated process receives a
///            signal to terminate.
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>            /// This file contains Linux system function call declarations.
#include <signal.h>            /// This file contains Linux signal processing declarations.
#include <stdexcept>           /// This file contains declarations for handling exceptions.
#include <string>              /// This file contains string handling utilities.
#include "SystemUtils.h"       /// This file contains Bose system utility declarations.
#include "DPrint.h"            /// This file contains the DPrint class declaration for logging.
#include "ProductController.h" /// This file contains the ProductController class declaration.

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger    { "Product" };
static const char   s_logName[] = "Product Start";

DPrint s_webExLogger( "Product" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProcessShutDown
///
/// @brief  This function is called whenever a termination signal by the system or by the user
///         (typically through a Ctrl+c key press) is received. Any actions that need to be taken
///         when the product controller shuts down should be handled from this function.
///
/// @param  signal [input] This integer argument stores the type of Linux signal sent.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessShutDown( int signal )
{
     if( signal == SIGTERM )
     {
         s_logger.LogInfo( "%-18s : The Product Controller is shutting down.                    ", s_logName );
         s_logger.LogInfo( "%-18s : The Product Controller received a system termination signal.", s_logName );

         exit( EXIT_SUCCESS );
     }
     else
     if( signal == SIGINT )
     {
         s_logger.LogInfo( "%-18s : The Product Controller is shutting down.                  ", s_logName );
         s_logger.LogInfo( "%-18s : The Product Controller received a user termination signal.", s_logName );

         exit( EXIT_SUCCESS );
     }
     else
     {
         s_logger.LogInfo( "%-18s : The Product Controller is shutting down.             ", s_logName );
         s_logger.LogInfo( "%-18s : The Product Controller received an unknown signal %d.", s_logName, signal );

         exit( EXIT_FAILURE );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   main
///
/// @brief  This function is the standard C++ main function called when the Product Controller
///         process first starts. It first assigns a function to handle any actions required
///         when the associated process receives a signal to terminate, and sets the process to
///         ignore any SIGPIPE signals that indicate that a failed write request took place, which
///         would normally cause the process to terminate. It then makes a system utility call to
///         ensure that only one instance of this process is running. Lastly, it gets a single
///         instance of a ProductController class to start and run the process. Note that any
///         exceptions generated by the Product Controller process that are not caught in lower
///         level functions or methods are caught in this main function.
///
/// @param  argumentCount [input] This integer argument stores the number of parameters passed to
///                               the command used to execute the Product Controller process. This
///                               number counts the name of the program itself as a parameter.
///
/// @param  argumentValue [input] This argument is a pointer to an array of strings (specified as
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
           signal( SIGPIPE, SIG_IGN );

           SystemUtils::ThereCanBeOnlyOne( );

           s_logger.LogInfo( "%-18s : The Product Controller is starting up.", s_logName );

           /////////////////////////////////////////////////////////////////////////////////////////
           /// @brief The Product Controller is now ran and the main task is suspended until the
           ///        task associated with the Product Controller is ended.
           /////////////////////////////////////////////////////////////////////////////////////////
           ProductController::GetInstance( )->Run ( );
           ProductController::GetInstance( )->Wait( );

           return( EXIT_SUCCESS );
    }
    catch( std::exception const& error )
    {
           s_logger.LogError( "%-18s : The Product Controller is shutting down.", s_logName );
           s_logger.LogError( "%-18s : An exception %-18s was caught.          ", s_logName, error.what( ) );

           return( EXIT_FAILURE );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
