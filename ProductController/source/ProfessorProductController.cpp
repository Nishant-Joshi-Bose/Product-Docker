////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProfessorProductController.cpp
///
/// @brief     This file contains source code that implements the ProfessorProductController class
///            that acts as a container to handle all the main functionality related to this program
///            that is product specific. In these regards, this class is used as a container to
///            control the product states, as well as to instantiate modules to manage the device
///            and lower level hardware, and interface with the user and system level applications.
///
/// @author    Stuart J. Lumby
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
#include "unistd.h"
#include "ProfessorProductController.h"
#include "ProductHardwareInterface.h"
#include "CustomProductAudioService.h"
#include "ProductKeyInputInterface.h"
#include "ProductVolumeManager.h"
#include "ProductEdidInterface.h"
#include "ProductNetworkManager.h"
#include "ProductSystemManager.h"
#include "ProductSpeakerManager.h"
#include "ProductCommandLine.h"
#include "ProductAdaptIQManager.h"
#include "ProductControllerStateTop.h"
#include "ProductControllerStateSetup.h"
#include "ProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateUpdatingSoftware.h"
#include "CustomProductControllerStateLowPower.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlayable.h"
#include "CustomProductControllerStateNetworkStandby.h"
#include "CustomProductControllerStateNetworkStandbyConfigured.h"
#include "CustomProductControllerStateNetworkStandbyUnconfigured.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateIdleVoiceConfigured.h"
#include "CustomProductControllerStateIdleVoiceUnconfigured.h"
#include "CustomProductControllerStatePlayingActive.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingInactive.h"
#include "CustomProductControllerStateAccessoryPairing.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS = 4;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::ProfessorProductController
///
/// @brief  This method is the ProfessorProductController constructor, which is declared as being
///         private to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProfessorProductController::ProfessorProductController( ) :

    ///
    /// Construction of the common Product Controller Class
    ///
    ProductController( "Professor" ),

    ///
    /// Construction of the Product Controller Modules
    ///
    m_ProductHardwareInterface( nullptr ),
    m_ProductSystemManager( nullptr ),
    m_ProductNetworkManager( nullptr ),
    m_ProductCommandLine( nullptr ),
    m_ProductKeyInputInterface( nullptr ),
    m_ProductEdidInterface( nullptr ),
    m_ProductVolumeManager( nullptr ),
    m_ProductAdaptIQManager( nullptr ),
    m_ProductSpeakerManager( nullptr ),
    m_ProductAudioService( nullptr ),

    ///
    /// Member Variable Initialization
    ///
    m_IsLpmReady( false ),
    m_IsCapsReady( false ),
    m_IsAudioPathReady( false ),
    m_IsSTSReady( false ),
    m_IsNetworkConfigured( false ),
    m_IsNetworkConnected( false ),
    m_IsAutoWakeEnabled( false ),
    m_IsAccountConfigured( false ),
    m_IsMicrophoneEnabled( false ),
    m_IsSoftwareUpdateRequired( false ),
    m_Running( false ),
    m_currentSource( SOURCE_TV )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::Run
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Run( )
{
    m_Running = true;

    BOSE_DEBUG( s_logger, "----------- Product Controller State Machine    ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller is setting up the state machine." );

    ///
    /// Construction of the Common States
    ///
    auto* stateTop = new ProductControllerStateTop( GetHsm( ),
                                                    nullptr );
    ///
    /// Construction of the Custom Professor States
    ///
    auto* stateBooting = new CustomProductControllerStateBooting
    ( GetHsm( ),
      stateTop,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );

    auto* stateUpdatingSoftware = new CustomProductControllerStateUpdatingSoftware
    ( GetHsm( ),
      stateTop,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING );

    auto* stateLowPower = new CustomProductControllerStateLowPower
    ( GetHsm( ),
      stateTop,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_LOW_POWER );

    auto* stateOn = new CustomProductControllerStateOn
    ( GetHsm( ),
      stateTop,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_ON );

    auto* statePlayable = new CustomProductControllerStatePlayable
    ( GetHsm( ),
      stateOn,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );

    auto* stateNetworkStandby = new CustomProductControllerStateNetworkStandby
    ( GetHsm( ),
      statePlayable,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );

    auto* stateNetworkStandbyConfigured = new CustomProductControllerStateNetworkStandbyConfigured
    ( GetHsm( ),
      stateNetworkStandby,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );

    auto* stateNetworkStandbyUnconfigured = new CustomProductControllerStateNetworkStandbyUnconfigured
    ( GetHsm( ),
      stateNetworkStandby,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );

    auto* stateIdle = new CustomProductControllerStateIdle
    ( GetHsm( ),
      statePlayable,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );

    auto* stateIdleVoiceConfigured = new CustomProductControllerStateIdleVoiceConfigured
    ( GetHsm( ),
      stateIdle,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );

    auto* stateIdleVoiceUnconfigured = new CustomProductControllerStateIdleVoiceUnconfigured
    ( GetHsm( ),
      stateIdle,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );

    auto* statePlaying = new CustomProductControllerStatePlaying
    ( GetHsm( ),
      stateOn,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING );

    auto* statePlayingActive = new CustomProductControllerStatePlayingActive
    ( GetHsm( ),
      statePlaying,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );

    auto* statePlayingInactive = new CustomProductControllerStatePlayingInactive
    ( GetHsm( ),
      statePlaying,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );

    auto* stateAccessoryPairing = new CustomProductControllerStateAccessoryPairing
    ( GetHsm( ),
      statePlayingActive,
      *this,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );

    ///
    /// The states are added to the state machine and the state machine is initialized.
    ///
    GetHsm( ).AddState( stateTop );
    GetHsm( ).AddState( stateBooting );
    GetHsm( ).AddState( stateUpdatingSoftware );
    GetHsm( ).AddState( stateLowPower );
    GetHsm( ).AddState( stateOn );
    GetHsm( ).AddState( statePlayable );
    GetHsm( ).AddState( stateNetworkStandby );
    GetHsm( ).AddState( stateNetworkStandbyConfigured );
    GetHsm( ).AddState( stateNetworkStandbyUnconfigured );
    GetHsm( ).AddState( stateIdle );
    GetHsm( ).AddState( stateIdleVoiceConfigured );
    GetHsm( ).AddState( stateIdleVoiceUnconfigured );
    GetHsm( ).AddState( statePlaying );
    GetHsm( ).AddState( statePlayingActive );
    GetHsm( ).AddState( statePlayingInactive );
    GetHsm( ).AddState( stateAccessoryPairing );

    GetHsm( ).Init( this, PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );

    ///
    /// Initialize entities in the Common Product Controller
    ///
    m_deviceManager.Initialize( this );

    ///
    /// Get instances of all the modules.
    ///
    BOSE_DEBUG( s_logger, "----------- Product Controller Starting Modules ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller instantiating and running its modules." );

    m_ProductHardwareInterface = std::make_shared< ProductHardwareInterface >( *this );
    m_ProductEdidInterface     = std::make_shared< ProductEdidInterface     >( *this );
    m_ProductSystemManager     = std::make_shared< ProductSystemManager     >( *this );
    m_ProductNetworkManager    = std::make_shared< ProductNetworkManager    >( *this );
    m_ProductCommandLine       = std::make_shared< ProductCommandLine       >( *this );
    m_ProductKeyInputInterface = std::make_shared< ProductKeyInputInterface >( *this );
    m_ProductVolumeManager     = std::make_shared< ProductVolumeManager     >( *this );
    m_ProductAdaptIQManager    = std::make_shared< ProductAdaptIQManager    >( *this );
    m_ProductSpeakerManager    = std::make_shared< ProductSpeakerManager    >( *this );
    m_ProductAudioService      = std::make_shared< CustomProductAudioService>( *this );

    if( m_ProductHardwareInterface == nullptr ||
        m_ProductSystemManager     == nullptr ||
        m_ProductNetworkManager    == nullptr ||
        m_ProductAudioService      == nullptr ||
        m_ProductCommandLine       == nullptr ||
        m_ProductKeyInputInterface == nullptr ||
        m_ProductEdidInterface     == nullptr ||
        m_ProductVolumeManager     == nullptr ||
        m_ProductAdaptIQManager    == nullptr )
    {
        BOSE_CRITICAL( s_logger, "-------- Product Controller Failed Initialization ----------" );
        BOSE_CRITICAL( s_logger, "A Product Controller module failed to be allocated.         " );

        return;
    }

    ///
    /// Run all the submodules.
    ///
    m_ProductHardwareInterface ->Run( );
    m_ProductSystemManager     ->Run( );
    m_ProductNetworkManager    ->Run( );
    m_ProductAudioService      ->Run( );
    m_ProductCommandLine       ->Run( );
    m_ProductKeyInputInterface ->Run( );
    m_ProductEdidInterface     ->Run( );
    m_ProductVolumeManager     ->Run( );
    m_ProductAdaptIQManager    ->Run( );
    m_ProductSpeakerManager    ->Run( );

    ///
    /// Register FrontDoor EndPoints
    ///
    RegisterFrontDoorEndPoints( );

    ///
    /// Set up the STSProductController
    ///
    SetupProductSTSConntroller( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetMessageHandler
///
/// @return Callback < ProductMessage >
///
////////////////////////////////////////////////////////////////////////////////////////////////////
Callback < ProductMessage > ProfessorProductController::GetMessageHandler( )
{
    Callback < ProductMessage >
    ProductMessageHandler( std::bind( &ProfessorProductController::HandleMessage,
                                      this,
                                      std::placeholders::_1 ) );
    return ProductMessageHandler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetCommandLineInterface
///
/// @return This method returns a reference to a command line interface for adding module specific
///         commands. Note that this interface is instantiated in the inherited ProductController
///         class; the ProductCommandLine interface instantiated in this class is used for specific
///         product controller commands in Professor.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CliClientMT& ProfessorProductController::GetCommandLineInterface( )
{
    return m_CliClientMT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetHardwareInterface
///
/// @return This method returns a shared pointer to the LPM hardware interface.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductHardwareInterface >& ProfessorProductController::GetHardwareInterface( )
{
    return m_ProductHardwareInterface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetVolumeManager
///
/// @return This method returns a shared pointer to the VolumeManager instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductVolumeManager >& ProfessorProductController::GetVolumeManager( )
{
    return m_ProductVolumeManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetAdaptIQManager
///
/// @return This method returns a shared pointer to the AdaptIQManager instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductAdaptIQManager >& ProfessorProductController::GetAdaptIQManager( )
{
    return m_ProductAdaptIQManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::GetSpeakerManager
///
/// @return This method returns a shared pointer to the ProductSpeakerManager instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductSpeakerManager >& ProfessorProductController::GetSpeakerManager( )
{
    return m_ProductSpeakerManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::GetEdidInterface
///
/// @return This method returns a shared pointer to the ProductEdidInterface instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductEdidInterface >& ProfessorProductController::GetEdidInterface( )
{
    return m_ProductEdidInterface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsBooted
///
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has booted.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsBooted( ) const
{
    BOSE_VERBOSE( s_logger, "------------ Product Controller Booted Check ---------------" );
    BOSE_VERBOSE( s_logger, " " );
    BOSE_VERBOSE( s_logger, "LPM Connected        :  %s", ( m_IsLpmReady       ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "CAPS Initialized     :  %s", ( m_IsCapsReady      ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Audio Path Connected :  %s", ( m_IsAudioPathReady ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "STS Initialized      :  %s", ( m_IsSTSReady       ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, " " );

    return ( m_IsLpmReady and m_IsCapsReady and m_IsAudioPathReady and m_IsSTSReady );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsNetworkConfigured
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsNetworkConfigured( ) const
{
    return m_IsNetworkConfigured;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsNetworkConfigured
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsNetworkConnected( ) const
{
    return m_IsNetworkConnected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsAutoWakeEnabled
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsAutoWakeEnabled( ) const
{
    return m_IsAutoWakeEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsVoiceConfigured
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsVoiceConfigured( ) const
{
    return ( m_IsMicrophoneEnabled and m_IsAccountConfigured );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsSoftwareUpdateRequired
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsSoftwareUpdateRequired( ) const
{
    return m_IsSoftwareUpdateRequired;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsSystemLanguageSet
///
/// @return This method returns true if the corresponding member has a system language defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsSystemLanguageSet( ) const
{
    return m_deviceManager.IsLanguageSet();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductType
///
/// @return This method returns the std::string const& value to be used for the Product "Type" field
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetProductType() const
{
    static std::string productType = "Professor Soundbar";
    return productType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductVariant
///
/// @return This method returns the std::string const& value to be used for the Product "Variant" field
///
/// @TODO - Below value may be available through HSP APIs
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetProductVariant() const
{
    static std::string productType = "Professor";
    return productType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::SetupProductSTSConntroller
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SetupProductSTSConntroller( )
{
    std::vector< ProductSTSController::SourceDescriptor > sources;

    ///
    /// Adapt IQ is not available as a normal source, whereas the TV source will always be available.
    ///
    ProductSTSController::SourceDescriptor descriptor_AiQ{ ProductSTS::SLOT_AIQ, "ADAPTiQ", false };
    ProductSTSController::SourceDescriptor descriptor_TV { ProductSTS::SLOT_TV,  "TV",      true  };

    sources.push_back( descriptor_AiQ );
    sources.push_back( descriptor_TV );

    Callback< void >
    CallbackForSTSComplete( std::bind( &ProfessorProductController::HandleSTSInitWasComplete,
                                       this ) );


    Callback< ProductSTSAccount::ProductSourceSlot >
    CallbackToHandleSelectSourceSlot( std::bind( &ProfessorProductController::HandleSelectSourceSlot,
                                                 this,
                                                 std::placeholders::_1 ) );

    m_ProductSTSController.Initialize( sources,
                                       CallbackForSTSComplete,
                                       CallbackToHandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleSTSInitWasComplete
///
/// @brief  This method is called from the ProductSTSController when all the initially-created
///         sources have been created with CAPS/STS.
///
/// @note   This method is called on the ProductSTSController task.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleSTSInitWasComplete( )
{
    ProductMessage message;
    message.mutable_stsinterfacestatus( )->set_initialized( true );

    IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                this,
                                message ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleSelectSourceSlot
///
/// @brief  This method is called from the ProductSTSController, when one of our sources is
///         activated by CAPS via STS.
///
/// @note   This method is called on the ProductSTSController task.
///
/// @param  ProductSTSAccount::ProductSourceSlot sourceSlot This identifies the activated slot.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot )
{
    ProductMessage message;
    message.mutable_selectsourceslot( )->set_slot( static_cast< ProductSTS::ProductSourceSlot >( sourceSlot ) );

    IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                this,
                                message ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::RegisterFrontDoorEndPoints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::RegisterFrontDoorEndPoints( )
{
    RegisterCommonEndPoints( );
    RegisterNowPlayingEndPoint( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::SendPlaybackRequest
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SendPlaybackRequest( PlaybackSource_t playbackSource )
{
    BOSE_INFO( s_logger, "Source %d was selected. \n", playbackSource );

    AsyncCallback< FRONT_DOOR_CLIENT_ERRORS >
    errorCallback( std::bind( &ProfessorProductController::PostPlaybackRequestError,
                              this,
                              std::placeholders::_1 ),
                   GetTask( ) );

    AsyncCallback< SoundTouchInterface::NowPlayingJson >
    postPlaybackRequestCallback( std::bind( &ProfessorProductController::PostPlaybackRequestResponse,
                                            this, std::placeholders::_1 ),
                                 GetTask( ) );
    ///
    /// Setup the playback request data.
    ///
    SoundTouchInterface::playbackRequestJson playbackRequestData;

    ///
    /// The data is hardcoded for test for now, before CAPS provides the utility to convert
    /// nowPlaying to playbackRequest.
    ///
    constexpr char source[ ]           = "DEEZER";
    constexpr char sourceAccount[ ]    = "aleksander_soltan@bose.com";
    constexpr char presetType[ ]       = "topTrack";
    constexpr char location[ ]         = "132";
    constexpr char name[ ]             = "Pop - ##TRANS_TopTracks##";
    constexpr bool presetable          = true;
    constexpr char containerArt[ ]     = "http://e-cdn-images.deezer.com/images/misc/db7a604d9e7634a67d45cfc86b48370a/500x500-000000-80-0-0.jpg";
    constexpr char playbackType[ ]     = "topTrack";
    constexpr char playbackLocation[ ] = "132";
    constexpr char playbackName[ ]     = "Too Good At Goodbyes";
    constexpr bool playbackPresetable  = true;

    switch( playbackSource )
    {
    case SOURCE_TV:
        playbackRequestData.set_source( "PRODUCT" );
        playbackRequestData.set_sourceaccount( "TV" );
        break;
    case SOURCE_SOUNDTOUCH:
        playbackRequestData.set_source( source );
        playbackRequestData.set_sourceaccount( sourceAccount );
        playbackRequestData.mutable_preset( )  -> set_type( presetType );
        playbackRequestData.mutable_preset( )  -> set_location( location );
        playbackRequestData.mutable_preset( )  -> set_name( name );
        playbackRequestData.mutable_preset( )  -> set_presetable( presetable );
        playbackRequestData.mutable_preset( )  -> set_containerart( containerArt );
        playbackRequestData.mutable_playback( )-> set_type( playbackType );
        playbackRequestData.mutable_playback( )-> set_location( playbackLocation );
        playbackRequestData.mutable_playback( )-> set_name( playbackName );
        playbackRequestData.mutable_playback( )-> set_presetable( playbackPresetable );
        break;
    }

    ///
    /// Send a post messgage for the "/content/playbackRequest" end point.
    ///
    m_FrontDoorClientIF->SendPost< SoundTouchInterface::NowPlayingJson >( "/content/playbackRequest",
                                                                          playbackRequestData,
                                                                          postPlaybackRequestCallback,
                                                                          errorCallback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetCurrentSource
///
/// @return This method returns the currently selected source.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
PlaybackSource_t ProfessorProductController::GetCurrentSource( )
{
    return m_currentSource;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::PostPlaybackRequestResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::PostPlaybackRequestResponse( const SoundTouchInterface::NowPlayingJson& response )
{
    BOSE_DEBUG( s_logger, "A response to the playback request %s was received." ,
                response.source( ).sourcedisplayname( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::PostPlaybackRequestError
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::PostPlaybackRequestError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_DEBUG( s_logger, "An error %d was returned to the playback request.", errorCode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::RegisterNowPlayingEndPoint
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::RegisterNowPlayingEndPoint( )
{
    ///
    /// Registration as a client for getting notification of changes in the now playing state from
    /// CAPS is made through the FrontDoorClient object pointer. The callback HandleCapsNowPlaying
    /// is used to receive these notifications.
    ///
    AsyncCallback< SoundTouchInterface::NowPlayingJson >
    callback( std::bind( &ProfessorProductController::HandleNowPlaying,
                         this, std::placeholders::_1 ),
              GetTask( ) );

    m_FrontDoorClientIF->RegisterNotification< SoundTouchInterface::NowPlayingJson >
    ( "/content/nowPlaying", callback );

    BOSE_DEBUG( s_logger, "A notification request for CAPS now playing status has been made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandleNowPlaying
///
/// @param SoundTouchInterface::NowPlayingJson& nowPlayingStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleNowPlaying( const SoundTouchInterface::NowPlayingJson&
                                                   nowPlayingStatus )
{
    BOSE_DEBUG( s_logger, "A CAPS now playing status has been received." );

    if( nowPlayingStatus.has_state( ) )
    {
        BOSE_DEBUG( s_logger, "The CAPS now playing status has a %s status.",
                    SoundTouchInterface::StatusJson_Name( nowPlayingStatus.state( ).status( ) ).c_str( ) );

        if( nowPlayingStatus.state( ).status( ) == SoundTouchInterface::StatusJson::play )
        {
            ProductMessage productMessage;
            productMessage.mutable_nowplayingstatus( )->set_state( ProductNowPlayingStatus_ProductNowPlayingState_Active );

            IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                        this,
                                        productMessage ),
                             GetTask( ) );
        }
        else
        {
            ProductMessage productMessage;
            productMessage.mutable_nowplayingstatus( )->set_state( ProductNowPlayingStatus_ProductNowPlayingState_Inactive );

            IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                        this,
                                        productMessage ),
                             GetTask( ) );
        }
    }
    else
    {
        BOSE_DEBUG( s_logger, "The CAPS now playing status is unknown." );
    }

    if( nowPlayingStatus.has_container( )                          and
        nowPlayingStatus.container( ).has_contentitem( )           and
        nowPlayingStatus.container( ).contentitem( ).has_source( ) and
        nowPlayingStatus.container( ).contentitem( ).has_sourceaccount( ) )
    {
        if( nowPlayingStatus.container( ).contentitem( ).source( ).compare( "PRODUCT" ) == 0   and
            nowPlayingStatus.container( ).contentitem( ).sourceaccount( ).compare( "TV" ) == 0 )
        {
            BOSE_DEBUG( s_logger, "The CAPS now playing source is set to SOURCE_TV." );

            m_currentSource = SOURCE_TV;
        }
        else
        {
            BOSE_DEBUG( s_logger, "The CAPS now playing source is set to SOURCE_SOUNDTOUCH." );

            m_currentSource = SOURCE_SOUNDTOUCH;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleMessage
///
/// @brief  This method is called to handle product controller messages, which are sent from the
///         more product specific class instances, and is used to process the state machine for the
///         product.
///
/// @param  ProductMessage& message This argument contains product message event information based
///                                 on the ProductMessage Protocal Buffer.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleMessage( const ProductMessage& message )
{
    BOSE_DEBUG( s_logger, "----------- Product Controller Message Handler -------------" );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( message.has_lpmstatus( ) )
    {
        if( message.lpmstatus( ).has_connected( ) )
        {
            m_IsLpmReady = message.lpmstatus( ).connected( );
            BOSE_DEBUG( s_logger, "An LPM Hardware %s message was received.",
                        m_IsLpmReady ? "up" : "down" );
            GetHsm( ).Handle< bool >
            ( &CustomProductControllerState::HandleLpmState, m_IsLpmReady );
        }

        if( message.lpmstatus( ).has_powerstatus( ) )
        {
            switch( message.lpmstatus( ).powerstatus( ) )
            {
            case ProductLpmStatus::ColdBooted:
                GetHsm( ).Handle<>( &CustomProductControllerState::HandleLPMPowerStatusColdBoot );
                break;
            case ProductLpmStatus::LowPower:
                GetHsm( ).Handle<>( &CustomProductControllerState::HandleLPMPowerStatusLowPower );
                break;
            case ProductLpmStatus::NetworkStandby:
                GetHsm( ).Handle<>( &CustomProductControllerState::HandleLPMPowerStatusNetworkStandby );
                break;
            case ProductLpmStatus::AutoWakeStandby:
                GetHsm( ).Handle<>( &CustomProductControllerState::HandleLPMPowerStatusAutoWakeStandby );
                break;
            case ProductLpmStatus::FullPower:
                GetHsm( ).Handle<>( &CustomProductControllerState::HandleLPMPowerStatusFullPower );
                break;
            default:
                break;
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Content Audio Playback Services (CAPS) status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_capsstatus( ) )
    {
        if( message.capsstatus( ).has_initialized( ) )
        {
            m_IsCapsReady = message.capsstatus( ).initialized( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid CAPS status message was received." );
            return;
        }

        BOSE_DEBUG( s_logger, "A CAPS Content Audio Playback Services %s message was received.",
                    m_IsCapsReady ? "up" : "down" );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleCapsState, m_IsCapsReady );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Audio path status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_audiopathstatus( ) )
    {
        if( message.audiopathstatus( ).has_connected( ) )
        {
            m_IsAudioPathReady = message.audiopathstatus( ).connected( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid audio path status message was received." );
            return;
        }

        BOSE_DEBUG( s_logger, "An audio path status %s message was received.",
                    m_IsAudioPathReady ? "connected" : "not connected" );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleAudioPathState, m_IsAudioPathReady );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// STS interface status is handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_stsinterfacestatus( ) )
    {
        BOSE_DEBUG( s_logger, "An STS Sources Initialized message was received." );

        m_IsSTSReady = true;

        GetHsm( ).Handle<>
        ( &CustomProductControllerState::HandleSTSSourcesInit );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// STS slot selected data is handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_selectsourceslot( ) )
    {
        const auto& slot = message.selectsourceslot( ).slot( );

        BOSE_DEBUG( s_logger, "An STS Select message was received for slot %s.",
                    ProductSourceSlot_Name( slot ).c_str( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Network status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_networkstatus( ) )
    {
        if( message.networkstatus( ).has_configured( ) )
        {
            m_IsNetworkConfigured = message.networkstatus( ).configured( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid network configured status message was received." );
            return;
        }

        if( message.networkstatus( ).has_connected( ) )
        {
            m_IsNetworkConnected = message.networkstatus( ).connected( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid network connected status message was received." );
            return;
        }

        if( message.networkstatus( ).networktype( ) == ProductNetworkStatus_ProductNetworkType_Wireless )
        {
            BOSE_DEBUG( s_logger, "A wireless %s %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "unconfigured",
                        m_IsNetworkConnected  ? "connected"  : "unconnected" );
        }
        else if( message.networkstatus( ).networktype( ) == ProductNetworkStatus_ProductNetworkType_Wired )
        {
            BOSE_DEBUG( s_logger, "A wired %s %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "unconfigured",
                        m_IsNetworkConnected  ? "connected"  : "unconnected" );
        }
        else
        {
            BOSE_DEBUG( s_logger, "A unknown %s %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "unconfigured",
                        m_IsNetworkConnected  ? "connected"  : "unconnected" );
        }

        m_ProductSystemManager->SetNetworkAccoutConfigurationStatus( m_IsNetworkConfigured,
                                                                     m_IsAccountConfigured );

        GetHsm( ).Handle< bool, bool >
        ( &CustomProductControllerState::HandleNetworkState, m_IsNetworkConfigured, m_IsNetworkConnected );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Wireless network status messages are handled at this point. Only send information to the
    /// state machine if the wireless network is configured, since a wired network configuration
    /// may be available, and is handle above.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_wirelessstatus( ) )
    {
        if( message.wirelessstatus( ).has_configured( ) && message.wirelessstatus( ).configured( ) )
        {
            m_IsNetworkConfigured = true;

            m_ProductSystemManager->SetNetworkAccoutConfigurationStatus( m_IsNetworkConfigured,
                                                                         m_IsAccountConfigured );

            GetHsm( ).Handle< bool, bool >
            ( &CustomProductControllerState::HandleNetworkState, m_IsNetworkConfigured, m_IsNetworkConnected );
        }

        ///
        /// Send the frequency information (if available) to the LPM to avoid any frequency
        /// interruption during a speaker Adapt IQ process.
        ///
        if( message.wirelessstatus( ).has_frequencykhz( ) and
            message.wirelessstatus( ).frequencykhz( ) > 0 )
        {
            m_ProductHardwareInterface->SendWiFiRadioStatus( message.wirelessstatus( ).frequencykhz( ) );
        }

        BOSE_DEBUG( s_logger, "A %s wireless network message was received with frequency %d kHz.",
                    message.wirelessstatus( ).configured( ) ? "configured" : "unconfigured",
                    message.wirelessstatus( ).has_frequencykhz( ) ?
                    message.wirelessstatus( ).frequencykhz( ) : 0 );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Voice messages for the Virtual Personal Assistant or VPA are handled at this point.          ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_voicestatus( ) )
    {
        if( message.voicestatus( ).has_microphoneenabled( ) )
        {
            m_IsMicrophoneEnabled = message.voicestatus( ).microphoneenabled( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid voice status message for the microphone status was received." );
            return;
        }

        if( message.voicestatus( ).has_accountconfigured( ) )
        {
            m_IsAccountConfigured = message.voicestatus( ).accountconfigured( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid voice status message for account configuration was received." );
            return;
        }

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleVoiceState, IsVoiceConfigured( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Key data messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_keydata( ) )
    {
        auto keyData = message.keydata( );
        auto keyString = KeyActionPb::KEY_ACTION_Name( keyData.action( ) );

        BOSE_INFO( s_logger, "The key action value %s (valued %d) was received.",
                   keyString.c_str( ),
                   keyData.action( ) );

        GetHsm( ).Handle< int >
        ( &CustomProductControllerState::HandleKeyAction, keyData.action( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Autowake messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_autowakestatus( ) )
    {
        if( message.autowakestatus( ).has_active( ) )
        {
            m_IsAutoWakeEnabled = message.autowakestatus( ).active( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid autowake status message was received." );
            return;
        }

        BOSE_DEBUG( s_logger, "An autowake status %s message has been received.",
                    m_IsAutoWakeEnabled ? "active" : "inactive" );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleAutowakeStatus, m_IsAutoWakeEnabled );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Accessory pairing messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_accessorypairing( ) )
    {
        GetHsm( ).Handle< ProductAccessoryPairing >
        ( &CustomProductControllerState::HandlePairingState, message.accessorypairing( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Now playing status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_nowplayingstatus( ) )
    {
        if( not message.nowplayingstatus( ).has_state( ) )
        {
            BOSE_ERROR( s_logger, "An invalid now playing status message was received." );
            return;
        }

        BOSE_DEBUG( s_logger, "A now playing %s state has been received.",
                    ProductNowPlayingStatus_ProductNowPlayingState_Name
                    ( message.nowplayingstatus( ).state( ) ).c_str( ) );

        GetHsm( ).Handle< const ProductNowPlayingStatus_ProductNowPlayingState & >
        ( &CustomProductControllerState::HandleNowPlayingStatus, message.nowplayingstatus( ).state( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Unknown message types are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else
    {
        BOSE_ERROR( s_logger, "An unknown message type was received." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::Wait
///
/// @brief  This method is called from a calling task to wait until the Product Controller process
///         ends. It is running from the main task that started the application.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Wait( )
{
    while( m_Running )
    {
        sleep( PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS );
    }

    ///
    /// Stop all the submodules.
    ///
    m_ProductHardwareInterface->Stop( );
    m_ProductSystemManager    ->Stop( );
    m_ProductNetworkManager   ->Stop( );
    m_ProductCommandLine      ->Stop( );
    m_ProductKeyInputInterface->Stop( );
    m_ProductEdidInterface    ->Stop( );
    m_ProductVolumeManager    ->Stop( );
    m_ProductAdaptIQManager   ->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::End
///
/// @brief  This method is called when the Product Controller process ends. It is used to set the
///         running member to false, which will invoke the Wait method idle loop to exit and perform
///         any necessary clean up.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::End( )
{
    BOSE_DEBUG( s_logger, "The Product Controller main task is stopping." );

    m_Running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
