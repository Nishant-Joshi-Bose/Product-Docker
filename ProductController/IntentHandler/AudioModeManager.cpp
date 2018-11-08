////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file     AudioModeManager.h
/// @brief    This source code file declares an audio manager class for implementing audio mode control
///           (toggling NORMAL and DIALOG modes) based on product-specific and remote-specific
///           key actions.
/// @author   Nicholas Craffey
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AudioModeManager.h"


namespace ProductApp
{

	AudioModeManager::AudioModeManager( NotifyTargetTaskIF&		 task,
						  				const CliClientMT&		 commandLineClient,
						  				const FrontDoorClientIF_t& frontDoorClient,
						  				ProductController&		 productController )
		: IntentManager( task, commandLineClient, frontDoorClient, productController )
		{
			BOSE_INFO( s_logger, "%s is being constructed.", "AudioModeManager" );

			settingsManager = new CustomAudioSettingsManager();
		}


	bool AudioModeManager::Handle( KeyHandlerUtil::ActionType_t& action )
	{
		BOSE_INFO( s_logger, "%s in %s", "AudioModeManager", __func__ );

		if( action == ( uint16_t )Action::ACTION_AUDIO_MODE_TOGGLE )
		{
			ProductPb::AudioMode mode = settingsManager->GetMode();

			if ( mode.value() == "DIALOG")
			{
				BOSE_INFO( s_logger, "Current mode: DIALOG" );
				mode.set_value( "NORMAL" );
			}
			else if ( mode.value() == "NORMAL" )
			{
				BOSE_INFO( s_logger, "Current mode: NORMAL" );
				mode.set_value( "DIALOG" );

			}
			else
			{
				BOSE_ERROR( s_logger, "Current AudioMode is unknown and cannot be toggled" );
				return false;
			}

			settingsManager->SetMode( mode );
			settingsManager->RefreshMode();
			return true;
		}
		else
		{
			BOSE_ERROR( s_logger, "Unexpected intent %d received", action );
			return false;
		}
	}
}