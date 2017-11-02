////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAccessoryPairing.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product accessory pairing state.
///
/// @author    Derek Richardson
///
/// @date      09/22/2017
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
#include "DPrint.h"
#include "CustomProductControllerStateAccessoryPairing.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductSpeakerManager.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START = 4 * 60 * 1000;
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY = 0 ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::CustomProductControllerStateAccessoryPairing
///
/// @param ProductControllerHsm& [ hsm ] - hsm reference
///
/// @param CHsmState* [ pSuperState ] - parent state
///
/// @param ProfessorProductController& [ productController ] - reference to product controller
///
/// @param Hsm::STATE [ stateId ] - enum for state identification
///
/// @param const std::string& [ name ] = name to call state
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateAccessoryPairing::CustomProductControllerStateAccessoryPairing( ProductControllerHsm&       hsm,
        CHsmState*                  pSuperState,
        ProfessorProductController& productController,
        Hsm::STATE                  stateId,
        const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name ),
      m_productController( productController ),
      m_timer( APTimer::Create( productController.GetTask( ), "AccessoryPairingTimer" ) )
{
    BOSE_INFO( s_logger, "The product accessory pairing state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairing::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The product accessory pairing state is being started." );
    ProductSpeakerManager* productSpeakerManager = m_productController.GetSpeakerManager( );
    productSpeakerManager->DoPairing( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairing::HandleTimeOut( )
{
    BOSE_INFO( s_logger, "A time out while pairing has occurred." );

    m_timer->Stop( );

    //  @todo  where to go?
    ChangeState( GetSuperId( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandlePairingState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandlePairingState( ProductAccessoryPairing pairingStatus )
{
    if( !pairingStatus.active() )
    {
        // Pairing ended from outside force go back to playing active our parent state
        // @todo  where to go?
        ChangeState( GetSuperId( ) );
    }
    else
    {
        m_timer->SetTimeouts( PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START,
                              PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY );

        m_timer->Start( std::bind( &CustomProductControllerStateAccessoryPairing::HandleTimeOut,
                                   this ) );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairing::HandleStateExit( )
{
    BOSE_INFO( s_logger, "The product accessory pairing state is being exited." );
    m_timer->Stop( );
    ProductSpeakerManager* productSpeakerManager = m_productController.GetSpeakerManager( );
    productSpeakerManager->StopPairing( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
