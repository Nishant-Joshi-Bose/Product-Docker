////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file     AudioModeManager.h
/// @brief    This header file declares an audio manager class for implementing audio mode control
///           (toggling NORMAL and DIALOG modes) based on product-specific and remote-specific
///           key actions.
/// @author   Nicholas Craffey
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///			  Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "IntentManager.h"
#include "Intents.h"
#include "FrontDoorClientIF.h"
#include "Utilities.h"
#include "CustomAudioSettingsManager.h"
#include "AudioSettings.pb.h"


namespace ProductApp
{
	
	class AudioModeManager : public IntentManager
	{
	private:
		CustomAudioSettingsManager* settingsManager;

	public:
		AudioModeManager( NotifyTargetTaskIF&		 task,
						  const CliClientMT&		 commandLineClient,
						  const FrontDoorClientIF_t& frontDoorClient,
						  ProductController&		 productController );

		~AudioModeManager( ) override
		{

		}

		bool Handle( KeyHandlerUtil::ActionType_t& action ) override;
	};
}