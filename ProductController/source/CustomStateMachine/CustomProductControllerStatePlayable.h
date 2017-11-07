////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayable.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playable state.
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
#include <string>
#include "ProductControllerState.h"
#include "ProductControllerStates.h"
#include "HsmState.h"
//#include "SoundTouchInterface/ContentSelectionService.pb.h"

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
class ProductControllerHsm;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductControllerStatePlayable
///
/// @brief This class is used for executing produce specific actions when in a playable state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayable : public ProductControllerState
{
public:

    CustomProductControllerStatePlayable( ProductControllerHsm&       hsm,
                                          CHsmState*                  pSuperState,
                                          Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE,
                                          const std::string&          name    = "CustomProductControllerStatePlayable" );

    ~CustomProductControllerStatePlayable( ) override
    {

    }

    void HandleStateEnter( ) override;
    void HandleStateStart( ) override;
    void HandleStateExit( )  override;

    bool HandlePowerState( ) override;
    bool HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState state ) override;
    bool HandleKeyAction( int action ) override;
    bool HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo ) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
