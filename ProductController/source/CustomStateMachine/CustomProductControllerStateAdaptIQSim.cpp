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
#include "CustomProductControllerStateAdaptIQSim.h"
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
      m_timer( APTimer::Create( productController.GetTask( ), "AdaptIQTimer" ) ),
      m_AdaptIQManager( productController.GetAdaptIQManager( ) )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::StartTimer
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::StartTimer( int timeout )
{
    m_timer->SetTimeouts( timeout, 0 );
    m_timer->Start( [ = ]( )
    {
        HandleTimeOut();
    } );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleStateStart( )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being started." );

    // set initial status and send initial notification
    m_status.set_smstate( "NA" );
    m_status.set_mode( "Booting" );
    m_status.set_currentlocation( ProductAdaptIQManager::ADAPTIQ_LOCATION_FIRST );
    m_status.set_currentspeaker( ProductAdaptIQManager::ADAPTIQ_SPEAKER_FIRST );
    m_status.set_hpconnected( true );
    m_status.set_errorcode( 0 );
    m_AdaptIQManager->SetDefaultProperties( m_status );
    m_AdaptIQManager->SetStatus( m_status );
    StartTimer( ADAPTIQ_BOOT_TIME );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleTimeOut( )
{
    if( m_status.smstate() == "Booting" )
    {
        // DSP booted, wait for "Advance"
        m_status.set_smstate( "Intro" );
        m_status.set_mode( "Running" );
        m_AdaptIQManager->SetStatus( m_status );
    }
    else if( m_status.smstate() == "Listening" )
    {
        // measurement complete, wait for "Advance"
        m_status.set_smstate( "Measurement Complete" );
        m_AdaptIQManager->SetStatus( m_status );
    }
    else if( m_status.smstate() == "Analysis" )
    {
        // analysis complete, start tearing down adapt iq
        m_status.set_smstate( "Analysis Complete" );
        m_AdaptIQManager->SetStatus( m_status );
        StartTimer( ADAPTIQ_EXIT_TIME );
    }
    else if( m_status.smstate() == "Analysis Complete" )
    {
        ChangeState( GetSuperId() );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus( const ProductAdaptIQStatus& aiqStatus )
{
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
    switch( cmd.action() )
    {
    case ProductAdaptIQControl::Start:
        break;

    case ProductAdaptIQControl::Cancel:
        break;

    case ProductAdaptIQControl::Advance:
        if( m_status.smstate() == "Intro" )
        {
            // got "Advance" in intro, go to listening and wait for measurement complete
            m_status.set_smstate( "Listening" );
            m_AdaptIQManager->SetStatus( m_status );
            StartTimer( ADAPTIQ_MEASUREMENT_TIME );
        }
        else if( m_status.smstate() == "Measurement Complete" )
        {
            // got "Advance" in measurement complete, update location / speaker
            if( m_status.currentlocation() == ProductAdaptIQManager::ADAPTIQ_LOCATION_LAST )
            {
                m_status.set_smstate( "Analysis" );
                StartTimer( ADAPTIQ_ANALYSIS_TIME );
            }
            else
            {
                m_status.set_smstate( "Listening" );
                m_status.set_currentlocation( m_status.currentlocation() + 1 );
                m_status.set_currentspeaker( m_status.currentspeaker() + 1 );
                m_AdaptIQManager->SetStatus( m_status );
                StartTimer( ADAPTIQ_MEASUREMENT_TIME );
            }
        }
        else
        {
            // wrong state, error
            m_status.set_smstate( "Error" );
            m_AdaptIQManager->SetDefaultProperties( m_status );
            m_AdaptIQManager->SetStatus( m_status );
        }

        break;

    case ProductAdaptIQControl::Previous:
        if( m_status.smstate() == "Measurement Complete" )
        {
            // got "Previous" in measurement complete, update location / speaker
            if( m_status.currentlocation() > ProductAdaptIQManager::ADAPTIQ_LOCATION_FIRST )
            {
                m_status.set_currentlocation( m_status.currentlocation() - 1 );
                m_status.set_currentspeaker( m_status.currentspeaker() - 1 );
            }
            m_status.set_smstate( "Listening" );
            m_AdaptIQManager->SetStatus( m_status );
            StartTimer( ADAPTIQ_MEASUREMENT_TIME );
        }
        else
        {
            // wrong state, error
            m_status.set_smstate( "Error" );
            m_AdaptIQManager->SetDefaultProperties( m_status );
            m_AdaptIQManager->SetStatus( m_status );
        }

        break;

    default:
        break;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
