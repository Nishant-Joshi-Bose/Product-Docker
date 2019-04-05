///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief  This file contains source code for product specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor
///         interaction
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"
#include "CustomAudioSettingsManager.h"
#include "ThermalMonitorTask.h"
#include "DataCollectionClientIF.h"

// Eddie audio settings
#include "EddieAudioSettings.pb.h"
#define PRODUCT_AUDIO_SETTINGS(concatStr) EddieAudioSettings_t##concatStr

namespace ProductApp
{
class CustomProductController;
class CustomProductAudioService: public ProductAudioService
{
public:

    /*! \brief Constructor.
     * \param productController Reference to the product controller.
     * \param frontDoorClient The FrontDoor client instance to which to register events.
     * \param lpmClient LPM client interface. Used for requesting amp status.
     */
    CustomProductAudioService( CustomProductController& productController,
                               const FrontDoorClientIF_t& frontDoorClient,
                               LpmClientIF::LpmClientPtr lpmClient );

    /*! \brief Destructor.
     */
    ~CustomProductAudioService();

    /*! \brief Enabled/disable thermal monitoring using ThermalMonitorTask.
     * \param enabled New thermal monitoring state.
     */
    void SetThermalMonitorEnabled( bool enabled );

private:

    /*! \brief FrontDoor handling function: register common front door events.
    */
    void RegisterFrontDoorEvents() override;

    /*! \brief APProduct handling function: register common audio path events.
     */
    void RegisterAudioPathEvents() override;

    /*! \brief APProduct handling function: Callback invoked when we get a
     *   RegisterForMainStreamAudioSettingsRequest from the AudioPath.
     */
    void GetMainStreamAudioSettingsCallback( std::string contentItem, const Callback<std::string, std::string> cb );

    /*! \brief APProduct handling function: Send all audio data to AudioPath.
     */
    void SendMainStreamAudioSettingsEvent();

    /*! \brief Pull latest audio settigs from AudioSettingsManager into m_mainStreamAudioSettings.
     */
    void FetchLatestAudioSettings();

    /*! \brief Callback function invoked when new thermal data is received in ThermalMonitorTask.
     */
    void ThermalDataReceivedCb( IpcSystemTemperatureData_t data );

    /*! \brief Converts a string for "mode" into an enum value.
     * \param modeName String to convert.
     * \return AudioSettingsAudioMode_t value for given string.
     */
    PRODUCT_AUDIO_SETTINGS( _AudioMode ) ModeNameToEnum( const std::string& modeName );


    //! Holds information that APProduct would like to know, including audio settings and thermal data.
    PRODUCT_AUDIO_SETTINGS() m_mainStreamAudioSettings;

    //! Manages and persists audio settings.
    std::unique_ptr<CustomAudioSettingsManager> m_audioSettingsMgr;

    //! Task used for polling LPM for thermal data.
    ThermalMonitorTask m_thermalTask;

    //
    // Front Door handlers
    //
    std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>        m_audioBassSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>      m_audioCenterSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioMode>>             m_audioModeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>      m_audioTrebleSetting;
    DataCollectionClientIF::DataCollectionClientPtr                 m_dataCollectionClient;

};

}// namespace ProductApp
