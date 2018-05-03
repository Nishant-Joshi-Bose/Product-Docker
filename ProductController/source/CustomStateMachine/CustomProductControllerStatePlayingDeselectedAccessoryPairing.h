////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingDeselectedAccessoryPairing.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product accessory pairing state when in a playing deselected superstate.
///
/// @author    Derek Richardson
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProductControllerState.h"
#include "HsmState.h"
#include "LpmServiceMessages.pb.h"
#include "APTimer.h"

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
/// @class CustomProductControllerStatePlayingDeselectedAccessoryPairing
///
/// @brief This class is used for executing produce specific actions when in an idle state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayingDeselectedAccessoryPairing : public ProductControllerState
{
public:

    CustomProductControllerStatePlayingDeselectedAccessoryPairing(
        ProductControllerHsm&       hsm,
        CHsmState*                  pSuperState,
        ProfessorProductController& productController,
        Hsm::STATE                  stateId,
        const std::string&          name = "PlayingDeselectedAccessoryPairing" );

    ~CustomProductControllerStatePlayingDeselectedAccessoryPairing( ) override
    {

    }

    void HandleStateStart( ) override;
    void HandleStateExit( )  override;
    bool HandlePairingState( ProductAccessoryPairing pairingStatus ) override;
    bool HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )  override;
    bool HandleAudioPathSelect( )                                    override;

private:

    // When switching between selected and deselcted pairing states we do not want to stop
    // This flag is used to signal that we should not send stop event and is cleared on exit
    bool m_stopPairingOnExit;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
