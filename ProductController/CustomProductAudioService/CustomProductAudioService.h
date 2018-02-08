///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for Eddie specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"
#include "EddieProductController.h"
#include "CustomAudioSettingsManager.h"
#include "ThermalMonitorTask.h"

namespace ProductApp
{
class CustomProductAudioService: public ProductAudioService
{
public:

    /*! \brief Constructor.
     * \param productController Reference to the Eddie product controller.
     * \param frontDoorClient The FrontDoor client instance to which to register events.
     * \param lpmClient LPM client interface. Used for requesting amp status.
     */
    CustomProductAudioService( EddieProductController& productController,
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
    void ThermalDataReceivedCb( const IpcSystemTemperatureData_t& data );


    //! Holds information that APProduct would like to know, including audio settings and thermal data.
    LpmServiceMessages::AudioSettings_t             m_mainStreamAudioSettings;

    //! Manages and persists audio settings.
    std::unique_ptr<CustomAudioSettingsManager>     m_audioSettingsMgr;

    //! Task used for polling LPM for thermal data.
    ThermalMonitorTask                              m_thermalTask;

    //
    // Front Door handlers
    //
    std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>        m_audioBassSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>      m_audioTrebleSetting;


};

}// namespace ProductApp
