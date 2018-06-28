////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateBooting.h
///
/// @brief     This header file declares functionality to process events that occur during the
///            product booting state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProductControllerStatePlayingSelectedSilentSourceInvalid.h"
#include "HsmState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Forward Class Declarations
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductControllerHsm;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief The CustomProductControllerStatePlayingSelectedSilentSourceInvalid Class
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayingSelectedSilentSourceInvalid : public ProductControllerStatePlayingSelectedSilentSourceInvalid
{
public:

    CustomProductControllerStatePlayingSelectedSilentSourceInvalid( ProductControllerHsm& hsm,
                                                                    CHsmState* pSuperState,
                                                                    Hsm::STATE stateId,
                                                                    const std::string& name = "CustomPlayingSelectedSilentSourceInvalid" );

    ~CustomProductControllerStatePlayingSelectedSilentSourceInvalid( ) override
    {

    }

    bool HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent ) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
