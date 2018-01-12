////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingActive.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
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
#include "ProductControllerStatePlayingActive.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

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
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductControllerStatePlayingActive
///
/// @brief This class is used for executing produce specific actions when in an playing active state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayingActive : public ProductControllerStatePlayingActive
{
public:

    CustomProductControllerStatePlayingActive
    ( ProductControllerHsm&       hsm,
      CHsmState*                  pSuperState,
      Hsm::STATE                  stateId,
      const std::string&          name    = "CustomProductControllerStatePlayingActive" );

    ~CustomProductControllerStatePlayingActive( ) override
    {

    }

    bool HandleNowPlayingStatus( const ProductNowPlayingStatus_ProductNowPlayingState& state ) override;
    bool HandleIntent( KeyHandlerUtil::ActionType_t action )                                   override;
    bool HandleIntentUserPower( KeyHandlerUtil::ActionType_t action )                          override;
    bool HandleIntentVolumeMuteControl( KeyHandlerUtil::ActionType_t action )                  override;
    bool HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t action )                     override;
    bool HandleIntentPlayback( KeyHandlerUtil::ActionType_t action )                           override;

private:

    void ProcessUserActivity( );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
