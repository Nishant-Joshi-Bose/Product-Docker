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
#include "CustomProductController.h"
#include "ProductMessage.pb.h"
#include "CustomProductAudioService.h"

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
/// @param CustomProductController& productController This argument references the product controller.
///
/// @param Hsm::STATE                  stateId           This enumeration represents the state ID.
///
/// @param const std::string&          name              This argument names the state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateAdaptIQ::
CustomProductControllerStateAdaptIQ( ProductControllerHsm&       hsm,
                                     CHsmState*                  pSuperState,
                                     CustomProductController&    productController,
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

    ///
    /// Disable source selection while in AdaptIQ.
    ///
    GetProductController( ).SendAllowSourceSelectMessage( false );
    m_completed = false;

    m_timer->SetTimeouts( ADAPTIQ_INACTIVITY_TIMEOUT, 0 );
    m_timer->Start( [ = ]( )
    {
        HandleTimeOut();
    } );

    GetProductController( ).GetVoiceServiceClient( ).Stop( );

    GetProductController( ).GetProductAudioServiceInstance( )->BootDSPImage( LpmServiceMessages::IpcImage_t::IMAGE_AIQ );
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
    /// Initiate cancellation of AdaptIQ, and proceed to Cancelling state
    ///
    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_CANCELLING );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus( const ProductAdaptIQStatus& aiqStatus )
{
    ProductPb::AdaptIQStatus frontDoorStatus;

    GetCustomProductController( ).GetAdaptIQManager( )->DSPToFrontDoorStatus( frontDoorStatus, aiqStatus );
    GetCustomProductController( ).GetAdaptIQManager( )->SetStatus( frontDoorStatus );

    ProductAdaptIQStatus& status = const_cast<ProductAdaptIQStatus&>( aiqStatus );
    if( status.mutable_status()->smstate() == LpmServiceMessages::IpcAiqState_t::AIQ_STATE_NOT_RUNNING )
    {

        HardwareIface( )->BootDSPImage( LpmServiceMessages::IpcImage_t::IMAGE_USER_APPLICATION );

        GetProductController( ).SendAllowSourceSelectMessage( true );

        ///
        /// flag the Exit handler that we're exiting due to something in the AiQ process
        /// (as opposed to a power state change)
        ///
        m_completed = true;

        ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );
    }

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

    if( m_completed )
    {
        ///
        /// If AIQ Completed, stop playback andre-enable source selection
        ///
        GetProductController( ).SendAllowSourceSelectMessage( true );
        GetProductController( ).SendStopPlaybackMessage( );
        GetProductController( ).GetVoiceServiceClient( ).Start( );
    }
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

    m_timer->SetTimeouts( ADAPTIQ_INACTIVITY_TIMEOUT, 0 );
    m_timer->Start( [ = ]( )
    {
        HandleTimeOut();
    } );


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
/// @brief  CustomProductControllerAdaptIQ::HandleIntentPowerToggle
///
/// @return This method returns a true Boolean value indicating that it has handled the
///         PowerToggle intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleIntentPowerToggle( )
{
    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_CANCELLING );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerAdaptIQ::HandleIntentPowerOff
///
/// @return This method returns a true Boolean value indicating that it has handled the
///         PowerOff intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleIntentPowerOff( )
{
    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_CANCELLING );
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
///
/// @brief CustomProductControllerStateAdaptIQ::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it is handling the speaker
///         pairing intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )

{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %s.", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
