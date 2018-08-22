////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      AccessorySoftwareInstallManager.cpp
///
/// @brief     This source code files implements a AccessorySoftwareInstallManager class that is used to
///            manage the software update rules for the Product Controller.
//             It will use inputs from
//             - CastleSoftwareUpdate through frontdoor API's
//             - PC state machine going into and out of IDLE
//             - Configuration that is stored on persistence.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "AccessorySoftwareInstallManager.h"
#include "ProductSoftwareInstallScheduler.h"
#include "ProductSoftwareInstallManager.h"
#include "ProductLogger.h"

///
/// Class Name Declaration for Logging
///
namespace
{
constexpr char CLASS_NAME[ ] = "AccessorySoftwareInstallManager";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

constexpr uint32_t MAX_RETRY_FOR_SOFTWARE_UPDATE_INITIATED = 10;
constexpr uint32_t ONE_SECOND_IN_MS                        = 1000;

constexpr uint32_t COVER_UP_SOFTWARE_INSTALL_TIMER_VAL     =  5 * ONE_SECOND_IN_MS;           // 5 seconds -> milliseconds


using namespace ProductSoftwareUpdatePB;

AccessorySoftwareInstallManager::
AccessorySoftwareInstallManager( NotifyTargetTaskIF*                                    task,
                                 ProductSoftwareInstallManager&                         productSoftwareInstallManager,
                                 ProductSoftwareInstallScheduler&                       productSoftwareInstallScheduler ):
    m_task( task ),
    m_productSoftwareInstallManager( productSoftwareInstallManager ),
    m_ProductSoftwareInstallScheduler( productSoftwareInstallScheduler ),
    m_TimerInstallSoftwareUpdate( APTimerFactory::CreateTimer( task,
                                                               "InstallSoftwareUpdateTimer" ) )
{
}

void AccessorySoftwareInstallManager::Initialize( std::shared_ptr< AsyncCallback<void> >&                callbackForInstall,
                                                  std::shared_ptr< CustomProductLpmHardwareInterface >&  productLpmHardwareInterface )
{
    BOSE_INFO( s_logger, "%s::%s ", CLASS_NAME, __FUNCTION__ );

    m_ProductLpmHardwareInterface = productLpmHardwareInterface;
    m_ProductNotifyCallbackForInstall = callbackForInstall; // Register Product Callback when Accessory Update needs to be triggered.

    RegisterLPMCallbacks( );
}

void AccessorySoftwareInstallManager::ProductSoftwareUpdateStateNotified( )
{
    BOSE_INFO( s_logger, "%s::%s ", CLASS_NAME, __FUNCTION__ );

    // We should re-assess whether we need to schedule ourselves based on the newly notified ProductSoftwareUpdateState
    HandleSpeakerSoftwareStatus( m_softwareStatusCache );
}

void AccessorySoftwareInstallManager::HandleSpeakerSoftwareStatus( LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t softwareStatus )
{
    BOSE_INFO( s_logger, "%s::%s: Accessory Software Status %s Progress %u %%", CLASS_NAME, __func__,
               IpcAccessorySpeakerSoftwareStatus_t_Name( softwareStatus.status( ) ).c_str( ),
               softwareStatus.progress( ) );

    m_softwareStatusCache = softwareStatus;

    ///
    /// If a system software update is pending and allowed, then do not process the accessory software update request.
    /// In this way, system software updates will take precedence over accessory software updates.
    ///
    if( m_productSoftwareInstallManager.IsSoftwareUpdatePending( ) && m_productSoftwareInstallManager.IsSoftwareUpdateAllowed( ) )
    {
        BOSE_WARNING( s_logger, "%s::%s System Software Update is already pending and allowed", CLASS_NAME, __func__ );
        return;
    }

    auto proceedWithSoftwareUpdateCb = [this]( void )
    {
        ProceedWithSoftwareUpdate( );
    };
    auto cb1 = std::make_shared<AsyncCallback<void> > ( proceedWithSoftwareUpdateCb, &GetTask( ) );

    if( IsSoftwareUpdatePending( ) )
    {
        BOSE_INFO( s_logger, "%s::%s Installation Pending", CLASS_NAME, __func__ );
        m_ProductSoftwareInstallScheduler.SetSoftwareUpdateIsPending( true, cb1 );
        m_ProductSoftwareInstallScheduler.StartSoftwareUpdateAlgorithm( );
    }
    else
    {
        m_ProductSoftwareInstallScheduler.SetSoftwareUpdateIsPending( false, cb1 );
    }
}

void AccessorySoftwareInstallManager::RegisterLPMCallbacks( )
{
    BOSE_INFO( s_logger, "%s::%s ", CLASS_NAME, __FUNCTION__ );

    auto LpmConnectionCb = [this]( bool connectionState )
    {
        if( connectionState )
        {
            ///
            /// Register for accessory speaker software status events.
            ///
            Callback< LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t > softwareStatusCB(
                std::bind( &AccessorySoftwareInstallManager::HandleSpeakerSoftwareStatus,
                           this,
                           std::placeholders::_1 ) );

            bool success = m_ProductLpmHardwareInterface->RegisterForLpmEvents< LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t > (
                               LpmServiceMessages::IPC_ACCESSORY_SPEAKER_SOFTWARE_STATUS, softwareStatusCB );

            BOSE_INFO( s_logger, "%s %s registered for accessory speaker software status events from the LPM hardware.", CLASS_NAME,
                       ( success ? "Successfully" : "Unsuccessfully" ) );
        }
    };

    m_ProductLpmHardwareInterface->RegisterForLpmConnection( LpmConnectionCb );
}

void AccessorySoftwareInstallManager::ProceedWithSoftwareUpdate( )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    if( !IsSoftwareUpdatePending( ) )
    {
        BOSE_WARNING( s_logger, "%s:%s: Looks like something changed recently, No Software Update Pending, Anymore", CLASS_NAME, __FUNCTION__ );
        return;
    }

    // Timer and retries are to work around the existence of transition states that do not HandleSoftwareUpdateInstall
    if( m_reTryMaxCount < MAX_RETRY_FOR_SOFTWARE_UPDATE_INITIATED )
    {
        m_reTryMaxCount++;
        BOSE_INFO( s_logger, "%s::%s Go ahead with software update: Attempt number = %d", CLASS_NAME, __func__, m_reTryMaxCount );
        InstallSoftwareUpdateTimer( )->SetTimeouts( COVER_UP_SOFTWARE_INSTALL_TIMER_VAL, 0 );
        InstallSoftwareUpdateTimer( )->Start( std::bind( &AccessorySoftwareInstallManager::ProceedWithSoftwareUpdate, this ) );

        ( *m_ProductNotifyCallbackForInstall )( );
    }
}

void AccessorySoftwareInstallManager::InitiateSoftwareInstall( )
{
    BOSE_INFO( s_logger, "%s::%s Accessory Update Initiated", CLASS_NAME, __func__ );
    InstallSoftwareUpdateTimer( )->Stop( );
    m_ProductSoftwareInstallScheduler.SoftwareInstallInitiated( );

    m_ProductLpmHardwareInterface->SendAccessorySoftwareUpdate( );
}

}
