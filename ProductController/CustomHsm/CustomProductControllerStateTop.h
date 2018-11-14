////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateTop.h
///
/// @brief     The header file declares the Top State for the Product Controller HSM. Any methods
///            getting called in this state indicate an unexpected call or an error.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProductControllerStateTop.h"
#include "ProductControllerStates.h"
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
/// @brief The CustomProductControllerStateTop Class
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateTop : public ProductControllerStateTop
{
public:

    CustomProductControllerStateTop( ProductControllerHsm& hsm,
                                     CHsmState*            pSuperState,
                                     Hsm::STATE            stateId = 0,
                                     const std::string&    name    = "Top" );

    ~CustomProductControllerStateTop( ) override
    {

    }

    bool HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent ) override;
    bool HandleAdaptIQControl( const ProductAdaptIQControl& control ) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
