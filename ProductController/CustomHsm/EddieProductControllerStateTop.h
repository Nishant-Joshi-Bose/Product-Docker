////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateTop.h
/// @brief  The Top State in the Eddie Product. Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateTop.h"
#include "ProductControllerStates.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class EddieProductControllerStateTop : public ProductControllerStateTop
{
public:
    EddieProductControllerStateTop( ProductControllerHsm& hsm,
                                    CHsmState* pSuperState,
                                    Hsm::STATE stateId = 0,
                                    const std::string& name = "TOP" );

    virtual ~EddieProductControllerStateTop()
    {
    }

    bool HandleModulesReady() override;
    bool HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo ) override;
    bool HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent ) override;
    bool HandleIntentCountDown( KeyHandlerUtil::ActionType_t intent ) override;
};
} // namespace ProductApp
