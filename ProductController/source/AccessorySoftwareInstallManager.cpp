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
#include "stdio.h"
#include "time.h"
#include "SystemUtils.h"
#include "PersistenceFiles.h"
#include "ProductConstants.h"
#include "ProductLogger.h"
#include "EndPointsDefines.h"

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
AccessorySoftwareInstallManager( NotifyTargetTaskIF*                                        task,
                                 std::shared_ptr< CustomProductLpmHardwareInterface >&  productLpmHardwareInterface,
                                 ProductSoftwareInstallManager&                         productSoftwareInstallManager,
                                 ProductSoftwareInstallScheduler&                       productSoftwareInstallScheduler ):
    m_task( task ),
    m_ProductLpmHardwareInterface( productLpmHardwareInterface ),
    m_productSoftwareInstallManager( productSoftwareInstallManager ),
    m_ProductSoftwareInstallScheduler( productSoftwareInstallScheduler ),
    m_TimerInstallSoftwareUpdate( APTimerFactory::CreateTimer( task,
                                                               "InstallSoftwareUpdateTimer" ) )
{
}

void AccessorySoftwareInstallManager::Initialize( std::shared_ptr< AsyncCallback<void> > callbackForInstall )
{
    BOSE_INFO( s_logger, "%s::%s ", CLASS_NAME, __FUNCTION__ );

    RegisterLPMCallbacks();

    m_ProductNotifyCallbackForInstall = callbackForInstall; // Register Product Callback when Accessory Update needs to be triggered.
}

void AccessorySoftwareInstallManager::Dump( std::ostringstream& oss ) const
{
#if 0
    oss << "Product Software Install Manager Status Dump" << std::endl;
    oss << "Software Update Allowed(From Madrid) = " << ( uint32_t ) m_isSoftwareUpdateAllowed << std::endl;
    oss << "Software Update Foreground = " << ( uint32_t ) IsSwUpdateForeground() << std::endl;
    oss << "Product Software Update Cache: Software Update Module Ready = " << ( uint32_t ) IsSoftwareUpdateReady() << std::endl;
    oss << "Product Software Update Cache: Software Update Pending = " << ( uint32_t ) IsSoftwareUpdatePending() << std::endl;
    oss << "Product Software Update Cache: Partial Software Update Pending = " << ( uint32_t ) IsPartialSoftwareUpdatePending() << std::endl;
    oss << "Product Software Update Cache: Local Software Update Pending = " << ( uint32_t ) IsLocalSoftwareUpdatePending() << std::endl;
    oss << "Product Software Update Cache: Software Update Deferrable = " << ( uint32_t ) IsSoftwareUpdateDeferrable() << std::endl;
    oss << "Product Software Update Cache: Immediate Software Update Needed = " << ( uint32_t ) IsImmediateSoftwareUpdateRequired() << std::endl;
#endif
}

#if 0
void AccessorySoftwareInstallManager::HandleSoftwareUpdateStatusCb( const SoftwareUpdateProto::StatusResponse& response )
{
    BOSE_INFO( s_logger, "%s::%s StatusResponse = %s ", CLASS_NAME, __func__, response.DebugString().c_str() );

    m_softwareUpdateStatusCache.CopyFrom( response );

    m_reTryMaxCount = 0; // This is a fresh status, don't reset everything, just the retry
    HandleSoftwareUpdate();
}

void AccessorySoftwareInstallManager::HandleSystemUpdateStartRequest( const SystemUpdateStartPb::SystemUpdateStart &req,
                                                                      const Callback<SystemUpdateStartPb::SystemUpdateStart> &resp,
                                                                      const Callback<FrontDoor::Error> &error )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    SetSoftwareUpdateIsAllowed( );
    resp( req );

    HandleSoftwareUpdate();
}
#endif
void AccessorySoftwareInstallManager::HandleSpeakerSoftwareStatus( LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t softwareStatus )
{
    BOSE_INFO( s_logger, "%s::%s: Accessory Software Status %s Current Version %s Installation Version %s Progress %u %%", CLASS_NAME, __func__,
               IpcAccessorySpeakerSoftwareStatus_t_Name( softwareStatus.status( ) ).c_str( ),
               softwareStatus.currentversion( ).c_str( ),
               softwareStatus.installationversion( ).c_str( ),
               softwareStatus.progress( ) );

    m_softwareStatusCache = softwareStatus;

    ///
    /// If a system software update is pending, then do not process the accessory software update request.
    /// In this way, system software updates will take precedent over accessory software updates.
    ///
    if( m_productSoftwareInstallManager.IsSoftwareUpdatePending( ) )
    {
        BOSE_WARNING( s_logger, "%s::%s System Software Update is already pending", CLASS_NAME, __func__ );
        return;
    }

    if( !m_productSoftwareInstallManager.IsSoftwareUpdateAllowed( ) )
    {
        BOSE_INFO( s_logger, "%s::%s Software Update Not Allowed:", CLASS_NAME, __func__ );
        return;
    }

    auto proceedWithSoftwareUpdateCb = [this]( void )
    {
        ProceedWithSoftwareUpdate( );
    };
    auto cb1 = std::make_shared<AsyncCallback<void> > ( proceedWithSoftwareUpdateCb, &GetTask() );

    if( IsSoftwareUpdatePending() )
    {
        BOSE_INFO( s_logger, "%s::%s Installation Pending", CLASS_NAME, __func__ );
        m_ProductSoftwareInstallScheduler.SetSoftwareUpdateIsPending( true, cb1 );
        m_ProductSoftwareInstallScheduler.StartSoftwareUpdateAlgorithm();
    }
    else
    {
        m_ProductSoftwareInstallScheduler.SetSoftwareUpdateIsPending( false, cb1 );
    }
}

void AccessorySoftwareInstallManager::RegisterLPMCallbacks()
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

            BOSE_INFO( s_logger, "%s registered for accessory speaker software status events from the LPM hardware.",
                       ( success ? "Successfully" : "Unsuccessfully" ) );
        }
    };

    m_ProductLpmHardwareInterface->RegisterForLpmConnection( LpmConnectionCb );
}

void AccessorySoftwareInstallManager::ProceedWithSoftwareUpdate()
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    ///
    /// If a system software update is pending, then do not process the accessory software update request.
    /// In this way, system software updates will take precedent over accessory software updates.
    ///
    if( m_productSoftwareInstallManager.IsSoftwareUpdatePending( ) )
    {
        BOSE_WARNING( s_logger, "%s::%s System Software Update is already pending", CLASS_NAME, __func__ );
        return;
    }

    if( !IsSoftwareUpdatePending() )
    {
        BOSE_INFO( s_logger, "%s:%s: Looks like something changed recently, No Software Update Pending, Anymore", CLASS_NAME, __FUNCTION__ );
        return;
    }

    // Timer and retries are to work around the existence of transition states that do not HandleSoftwareUpdateInstall
    if( m_reTryMaxCount < MAX_RETRY_FOR_SOFTWARE_UPDATE_INITIATED )
    {
        m_reTryMaxCount++;
        BOSE_INFO( s_logger, "%s::%s Go ahead with software update: Attempt number = %d", CLASS_NAME, __func__, m_reTryMaxCount );
        InstallSoftwareUpdateTimer()->SetTimeouts( COVER_UP_SOFTWARE_INSTALL_TIMER_VAL, 0 );
        InstallSoftwareUpdateTimer()->Start( std::bind( &AccessorySoftwareInstallManager::ProceedWithSoftwareUpdate, this ) );

        ( *m_ProductNotifyCallbackForInstall )();
    }
}

}
