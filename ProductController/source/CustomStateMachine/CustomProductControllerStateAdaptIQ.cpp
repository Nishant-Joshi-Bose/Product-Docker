////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAdaptIQ.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            AdaptIQ state.
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
#include "Utilities.h"
#include "CustomProductControllerStateAdaptIQ.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::CustomProductControllerStateAdaptIQ
///
/// @param ProductControllerHsm&       hsm               This argument references the state machine.
///
/// @param CHsmState*                  pSuperState       This argument references the parent state.
///
/// @param ProfessorProductController& productController This argument references the product controller.
///
/// @param Hsm::STATE                  stateId           This enumeration represents the state ID.
///
/// @param const std::string&          name              This argument names the state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateAdaptIQ::CustomProductControllerStateAdaptIQ

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name ),
      m_timer( APTimer::Create( productController.GetTask( ), "AdaptIQTimer" ) )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleStateStart( )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being started." );

    m_timer->SetTimeouts( ADAPTIQ_INACTIVITY_TIMEOUT, 0 );
    m_timer->Start( [ = ]( )
    {
        HandleTimeOut();
    } );

    // TODO: does response come back after dsp has rebooted or as an event later on?
    HardwareIface( )->BootDSPImage( LpmServiceMessages::DSPImage_t::INSTALLER_IMAGE );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleTimeOut( )
{
    BOSE_INFO( s_logger, "A time out during AdaptIQ has occurred." );

    ///
    /// Go to the superstate of this state, which should be the last state that the
    /// product controller was in, to resume functionality.
    ///
    ChangeState( GetSuperId( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus( const ProductAdaptIQStatus& aiqStatus )
{
    ProductPb::AdaptIQStatus frontDoorStatus;
//    LpmServiceMessages::IpcAiqSetupStatus_t dspStatus = *aiqStatus.mutable_status();

    GetCustomProductController( ).GetAdaptIQManager( )->DSPToFrontDoorStatus( frontDoorStatus, aiqStatus.mutable_status() );
//    GetCustomProductController( ).GetAdaptIQManager( )->SetStatus( ProductAdaptIQControl::Start );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleStateExit( )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being exited." );
    m_timer->Stop( );

    // TODO: does response come back after dsp has rebooted or as an event later on?
    HardwareIface( )->BootDSPImage( LpmServiceMessages::DSPImage_t::END_USER_IMAGE );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerAdaptIQ::HandleAdaptIQControl
///
/// @return This method returns a true Boolean value indicating that it has handled the
///         AdaptIQControl request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQControl( const ProductAdaptIQControl& cmd )
{
    BOSE_INFO( s_logger, "%s : Handle Action %d\n", __func__, cmd.action() );

    // for now just forward the action on the the lpm / dsp; we'll do more complex stuff later
    switch( cmd.action() )
    {
    case ProductAdaptIQControl::Start:
        BOSE_INFO( s_logger, "%s : Start %d\n", __func__, cmd.action() );
        GetCustomProductController( ).GetAdaptIQManager( )->SendAdaptIQControl( ProductAdaptIQControl::Start );
        break;

    case ProductAdaptIQControl::Cancel:
        BOSE_INFO( s_logger, "%s : Cancel %d\n", __func__, cmd.action() );
        GetCustomProductController( ).GetAdaptIQManager( )->SendAdaptIQControl( ProductAdaptIQControl::Cancel );
        break;

    case ProductAdaptIQControl::Advance:
        BOSE_INFO( s_logger, "%s : Advance %d\n", __func__, cmd.action() );
        GetCustomProductController( ).GetAdaptIQManager( )->SendAdaptIQControl( ProductAdaptIQControl::Advance );
        break;

    case ProductAdaptIQControl::Previous:
        BOSE_INFO( s_logger, "%s : Previous %d\n", __func__, cmd.action() );
        GetCustomProductController( ).GetAdaptIQManager( )->SendAdaptIQControl( ProductAdaptIQControl::Previous );
        break;

    default:
        BOSE_INFO( s_logger, "%s : Unhandled Action %d\n", __func__, cmd.action() );
        break;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HardwareIface
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<CustomProductLpmHardwareInterface>& CustomProductControllerStateAdaptIQ::HardwareIface( )
{
    return GetCustomProductController( ).GetLpmHardwareInterface( );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
