////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAdaptIQCancelling.cpp
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
#include "CustomProductControllerStateAdaptIQCancelling.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
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
/// @brief CustomProductControllerStateAdaptIQCancelling::CustomProductControllerStateAdaptIQCancelling
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
CustomProductControllerStateAdaptIQCancelling::CustomProductControllerStateAdaptIQCancelling(
    ProductControllerHsm& hsm,
    CHsmState*            pSuperState,
    Hsm::STATE            stateId,
    const std::string&    name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQCancelling::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQCancelling::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    GetCustomProductController( ).GetAdaptIQManager( )->SendAdaptIQControl( ProductAdaptIQControl::Cancel );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQCancelling::HandleStateExit( )
{
    ///
    /// Re-enable source selection when cancelling AdaptIQ.
    ///
    GetProductController( ).SendAllowSourceSelectMessage( true );

    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQCancelling is being exited." );

    GetProductController( ).SendStopPlaybackMessage( );

    GetProductController( ).GetVoiceServiceClient( ).Start( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQCancelling::HandleAdaptIQStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQCancelling::HandleAdaptIQStatus( const ProductAdaptIQStatus& aiqStatus )
{
    ProductPb::AdaptIQStatus frontDoorStatus;

    GetCustomProductController( ).GetAdaptIQManager( )->DSPToFrontDoorStatus( frontDoorStatus, aiqStatus );
    GetCustomProductController( ).GetAdaptIQManager( )->SetStatus( frontDoorStatus );

    ProductAdaptIQStatus& status = const_cast<ProductAdaptIQStatus&>( aiqStatus );
    if( status.mutable_status()->smstate() == LpmServiceMessages::IpcAiqState_t::AIQ_STATE_NOT_RUNNING )
    {
        HardwareIface( )->BootDSPImage( LpmServiceMessages::IpcImage_t::IMAGE_USER_APPLICATION );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQCancelling::HandleDspBooted
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQCancelling::HandleDspBooted( const LpmServiceMessages::IpcDeviceBoot_t& dspBooted )
{
    ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_STOPPING_STREAMS );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQCancelling::HardwareIface
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<CustomProductLpmHardwareInterface>& CustomProductControllerStateAdaptIQCancelling::HardwareIface( )
{
    return GetCustomProductController( ).GetLpmHardwareInterface( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQCancelling::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it is handling the speaker
///         pairing intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQCancelling::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )

{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAdaptIQCancelling::HandleIntentVolumeControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the volume
///         control intent. It is ignored when cancelling accessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQCancelling::HandleIntentVolumeControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %s.", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAdaptIQCancelling::HandleIntentMuteControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the muting
///         intent. It is ignored when cancelling accessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQCancelling::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %s.", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
