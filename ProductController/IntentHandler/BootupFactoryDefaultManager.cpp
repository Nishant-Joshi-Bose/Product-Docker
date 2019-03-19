////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      BootupFactoryDefaultManager.cpp
/// @brief     This source code file declares a Bootup Factory Default manager class for implementing the triggering of
///            Factory Defautlt soon after boot based on key actions.
/// @author    Yishai Sered
////////////////////////////////////////////////////////////////////////////////////////////////////;

#include "BootupFactoryDefaultManager.h"
#include "CustomProductController.h"

namespace
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
// These numbers define the time window after exiting the BOOTUP state where a press and subsequent
// release of the "action" key can initiate a factory default.  The conditions that must be satisifed
// in order for a factory default to be initiated are
//  1) The system must *not* be communicating with a Kepler remote
//  2) The action button must be pressed within 5 seconds of exiting the bootup state
//  3) The action button must be held for between 15 and 45 seconds
constexpr int64_t   BOOTUP_FACTORY_DEFAULT_WINDOW_MSEC      = 5000;
constexpr int64_t   BOOTUP_FACTORY_DEFAULT_MIN_HOLD_MSEC    = 15000;
constexpr int64_t   BOOTUP_FACTORY_DEFAULT_MAX_HOLD_MSEC    = 45000;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief BootupFactoryDefaultManager::BootupFactoryDefaultManager
/// @param NotifyTargetTaskIF&        task
/// @param const CliClientMT&         commandLineClient
/// @param const FrontDoorClientIF_t& frontDoorClient
/// @param ProductController&         productController
////////////////////////////////////////////////////////////////////////////////////////////////////
BootupFactoryDefaultManager::BootupFactoryDefaultManager( NotifyTargetTaskIF&        task,
                                                          const CliClientMT&         commandLineClient,
                                                          const FrontDoorClientIF_t& frontDoorClient,
                                                          ProductController&         productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_productController( static_cast<CustomProductController&>( productController ) )
{
    BOSE_INFO( s_logger, "%s is being constructed.", __func__ );

}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  BootupFactoryDefaultManager::Handle
/// @brief  This method is used to handle key actions and decide when to generate a Factory Default action
/// @param  KeyHandlerUtil::ActionType_t& action
/// @return This method returns true base on its handling of the key action sent.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool BootupFactoryDefaultManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s received %s", __func__, CommonIntentHandler::GetIntentName( action ).c_str( ) );

    if( action == ( KeyHandlerUtil::ActionType_t )Action::BOOTUP_FACTORY_DEFAULT_START )
    {
        int64_t timeSinceBooted = MonotonicClock::NowMs( ) - m_productController.GetBootCompleteTime( );

        if( ( m_productController.GetBootCompleteTime( ) != 0 ) && ( timeSinceBooted < BOOTUP_FACTORY_DEFAULT_WINDOW_MSEC ) && !m_productController.IsBLERemoteConnected( ) )
        {
            // we allow the key start time to be latched anywhere within the window such that
            // if a user were to press the key, release it, and press it again within the window,
            // the time of the last press would be latched; this should prevent frustration in case
            // the user doesn't get a "solid touch" immediately
            BOSE_INFO( s_logger, "%s: Received bootup factory default press within valid window [%lld, %lld]",
                       __PRETTY_FUNCTION__, m_productController.GetBootCompleteTime( ), timeSinceBooted );
            m_bootupFactoryDefaultKeyTime = MonotonicClock::NowMs( );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s: Recieved bootup factory default press outside valid window [%lld, %lld]",
                          __PRETTY_FUNCTION__, m_productController.GetBootCompleteTime( ), timeSinceBooted );
        }
    }
    else if( ( action == ( KeyHandlerUtil::ActionType_t )Action::BOOTUP_FACTORY_DEFAULT_COMPLETE ) && ( m_bootupFactoryDefaultKeyTime != 0 ) )
    {
        int64_t timeSinceBootupFactoryDefaultRequest = MonotonicClock::NowMs( ) - m_bootupFactoryDefaultKeyTime;
        BOSE_INFO( s_logger, "%s: bootup intent check %lld", __PRETTY_FUNCTION__, timeSinceBootupFactoryDefaultRequest );

        if(
            ( timeSinceBootupFactoryDefaultRequest > BOOTUP_FACTORY_DEFAULT_MIN_HOLD_MSEC ) &&
            ( timeSinceBootupFactoryDefaultRequest < BOOTUP_FACTORY_DEFAULT_MAX_HOLD_MSEC ) )
        {
            BOSE_INFO( s_logger, "%s: Initiating bootup factory default", __PRETTY_FUNCTION__ );
            const KeyHandlerUtil::ActionType_t startFactoryDefault = static_cast< KeyHandlerUtil::ActionType_t >( Action::FACTORY_DEFAULT );
            ProductMessage message;
            message.set_action( startFactoryDefault );

            m_productController.SendAsynchronousProductMessage( message );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s: Cancelling window for bootup factory default [%lld]", __PRETTY_FUNCTION__, timeSinceBootupFactoryDefaultRequest );
        }
        // reset so we can only do this once
        m_bootupFactoryDefaultKeyTime = 0;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
