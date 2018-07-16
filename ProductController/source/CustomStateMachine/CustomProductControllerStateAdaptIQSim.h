////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAdaptIQ.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product accessory pairing state.
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
#include "ProductControllerStateIdle.h"
#include "LpmServiceMessages.pb.h"
#include "APTimer.h"
#include "ProductAdaptIQManager.h"

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
/// @class CustomProductControllerStateAdaptIQ
///
/// @brief This class is used for executing produce specific actions when in an idle state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateAdaptIQ : public ProductControllerState
{
public:

    CustomProductControllerStateAdaptIQ( ProductControllerHsm&       hsm,
                                         CHsmState*                  pSuperState,
                                         ProfessorProductController& productController,
                                         Hsm::STATE                  stateId,
                                         const std::string&          name    = "CustomAdaptIQSim" );

    ~CustomProductControllerStateAdaptIQ( ) override
    {

    }

    void HandleStateStart( ) override;
    void HandleStateExit( )  override;
    bool HandleAdaptIQStatus( const ProductAdaptIQStatus& aiqStatus ) override;
    bool HandleAdaptIQControl( const ProductAdaptIQControl& cmd );
    bool HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )  override;

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method will be invoked by an expired timer, which is defined above and armed on
    ///        a successful callback saying pairing was entered
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleTimeOut( );
    void StartTimer( int timeout );

    APTimerPtr m_timer;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This holds the current status of the AdaptIQ process.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductPb::AdaptIQStatus m_status;

    // This is just a placeholder
    static constexpr uint32_t ADAPTIQ_INACTIVITY_TIMEOUT    = 1 * 60 * 1000;

    static constexpr uint32_t ADAPTIQ_BOOT_TIME             = 1 * 1000;
    static constexpr uint32_t ADAPTIQ_MEASUREMENT_TIME      = 3 * 1000;
    static constexpr uint32_t ADAPTIQ_ANALYSIS_TIME         = 3 * 1000;
    static constexpr uint32_t ADAPTIQ_EXIT_TIME             = 1 * 1000;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
