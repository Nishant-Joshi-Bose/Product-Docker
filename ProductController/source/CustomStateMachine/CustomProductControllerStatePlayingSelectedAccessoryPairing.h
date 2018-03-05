////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingSelectedAccessoryPairing.h
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
#include "SoundTouchInterface/PlayerService.pb.h"

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
/// @class CustomProductControllerStatePlayingSelectedAccessoryPairing
///
/// @brief This class is used for executing produce specific actions when in an idle state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayingSelectedAccessoryPairing : public ProductControllerState
{
public:

    CustomProductControllerStatePlayingSelectedAccessoryPairing(
        ProductControllerHsm&       hsm,
        CHsmState*                  pSuperState,
        ProfessorProductController& productController,
        Hsm::STATE                  stateId,
        const std::string&          name = "PlayingSelectedAccessoryPairing" );

    ~CustomProductControllerStatePlayingSelectedAccessoryPairing( ) override
    {

    }

    void HandleStateStart( ) override;
    void HandleStateExit( )  override;
    bool HandlePairingState( ProductAccessoryPairing pairingStatus ) override;
    bool HandleAudioPathDeselect( )                                  override;
    bool HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo ) override;

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
