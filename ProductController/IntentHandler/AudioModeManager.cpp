////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file     AudioModeManager.cpp
/// @brief    This source code file declares an audio manager class for implementing audio mode control
///           (toggling NORMAL and DIALOG modes) based on key actions.
/// @author   Nicholas Craffey
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AudioModeManager.h"
#include "CustomProductController.h"

namespace ProductApp
{

AudioModeManager::AudioModeManager( NotifyTargetTaskIF&      task,
                                    const CliClientMT&       commandLineClient,
                                    const FrontDoorClientIF_t& frontDoorClient,
                                    ProductController&       productController )
    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_ProductController( productController )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "AudioModeManager" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Handling of the AudioModeToggle intent by calling the ToggleAudioMode method in
///        CustomProductAudioService
/// /////////////////////////////////////////////////////////////////////////////////////////////////
bool AudioModeManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s in %s", "AudioModeManager", __func__ );

    if( action == ( uint16_t )Action::ACTION_AUDIO_MODE_TOGGLE )
    {

        GetCustomProductController().GetProductAudioServiceInstance()->ToggleAudioMode();
        return true;
    }
    else
    {
        BOSE_ERROR( s_logger, "Unexpected intent %s received", CommonIntentHandler::GetIntentName( action ).c_str( ) );
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief AudioModeManager::GetCustomProductController
/// @return This method returns the custom product controller instance.
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductController& AudioModeManager::GetCustomProductController( ) const
{
    return static_cast<CustomProductController&>( m_ProductController );
}
}
