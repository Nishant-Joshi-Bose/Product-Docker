////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateTop.h
///
/// @brief     The source code file implements the Top State for the Product Controller HSM. Any
///            methods getting called in this state indicate an unexpected call or an error.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerStateTop.h"
#include "ProtoToMarkup.h"
#include "ProductLogger.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateTop::CustomProductControllerStateTop
///
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateTop::CustomProductControllerStateTop( ProductControllerHsm& hsm,
                                                                  CHsmState* pSuperState,
                                                                  Hsm::STATE stateId,
                                                                  const std::string& name ) :
    ProductControllerStateTop( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateTop::HandleAdaptIQControl
///
/// @param  const ProductAdaptIQControl& cmd
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateTop::HandleAdaptIQControl( const ProductAdaptIQControl& cmd )
{
    BOSE_INFO( s_logger, "The %s state is in %s handling %s.", GetName( ).c_str( ), __func__,
               ProductAdaptIQControl_AdaptIQAction_Name( cmd.action() ).c_str( ) );

    if( cmd.action() == ProductAdaptIQControl::Start )
    {
        s_ActionPendingFromTansition = ( KeyHandlerUtil::ActionType_t )Action::ACTION_APAPTIQ_START;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateTop::HandleIntentSpeakerPairing
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateTop::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "%s in %s is handling the intent action %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    if( intent == ( KeyHandlerUtil::ActionType_t )Action::ACTION_START_PAIR_SPEAKERS_LAN )
    {
        s_ActionPendingFromTansition = intent;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
