////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      Utilities.h
///
/// @brief     This file declares general constants or utilities for use by the product controller
///            code.
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
#include "DPrint.h"
#include "Callback.h"
#include "APClientSocketListenerIF.h"
#include "APClientSocketListenerFactory.h"
#include "APServerSocketListenerIF.h"
#include "APServerSocketListenerFactory.h"
#include "IPCMessageRouterIF.h"
#include "CliClient.h"
#include "CliClientMT.h"
#include "ContentSelectionService.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following definition determines the compile time log level. Any log messages that are
///        above the given level are compiled out of the executable and cannot be enabled at
///        run-time. Currently, the level is set to the highest level to output all log statements,
///        but it can be changed to a lower level to leave log statements above this level out of
///        the compiled application to reduce its overall size.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  BOSE_LOG_LEVEL
#undef  BOSE_LOG_LEVEL
#define BOSE_LOG_LEVEL ( DPrint::INSANE )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// This is the global product logger (instantiated in ProductStart.cpp).
///
////////////////////////////////////////////////////////////////////////////////////////////////////
extern DPrint& GetProductLogger();
static DPrint& s_logger = GetProductLogger();

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch SDK utilities for inter-process
///        and inter-thread communications.
///
//////////////////////////////////////////////////////////////////////////////////////////////
typedef APClientSocketListenerIF::ListenerPtr   ClientListener;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::ListenerPtr   ServerListener;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr MessageRouter;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// This is an enumeration of possible playback sources.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    SOURCE_TV,
    SOURCE_SOUNDTOUCH
}
PlaybackSource_t;

// TODO: this will be moved to the common product class ProductSourceInfo after SOS
// (this is already done on master, see PGC-1169 and associated pull requests)
inline bool IsSourceAvailable( const SoundTouchInterface::Sources_SourceItem& source )
{
    return source.status( ) == SoundTouchInterface::SourceStatus::AVAILABLE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
