////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductKeyInputManager.cpp
///
/// @brief     This source code file implements a CustomProductKeyInputManager class that inherits
///            the base ProductKeyInputManager class to manage receiving and processing of raw key
///            input from the LPM hardware.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "EddieProductController.h"
#include "CustomProductKeyInputManager.h"

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
constexpr const char* KEY_CONFIGURATION_FILE_NAME = "/var/run/KeyConfiguration.json";

static DPrint s_logger( "CustomProductKeyInputManager" );
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductKeyInputManager::CustomProductKeyInputManager
///
/// @param EddieProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductKeyInputManager::CustomProductKeyInputManager( EddieProductController& ProductController )

    : ProductKeyInputManager( ProductController.GetTask( ),
                              ProductController.GetMessageHandler( ),
                              ProductController.GetLpmHardwareInterface( ),
                              ProductController.GetCommandLineInterface( ),
                              ProductController.GetFrontDoorClient( ),
                              KEY_CONFIGURATION_FILE_NAME ),

      m_ProductController( ProductController )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductKeyInputManager::Initialize
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductKeyInputManager::Initialize()
{
    BOSE_INFO( s_logger, "CustomProductKeyInputManager::%s:", __func__ );

    m_keyIdToKeyNameMap[1]  = KeyNamesPB::keynames::BLUETOOTH;
    m_keyIdToKeyNameMap[2]  = KeyNamesPB::keynames::AUX;
    m_keyIdToKeyNameMap[3]  = KeyNamesPB::keynames::VOLUME_UP;
    m_keyIdToKeyNameMap[4]  = KeyNamesPB::keynames::MFB;
    m_keyIdToKeyNameMap[5]  = KeyNamesPB::keynames::VOLUME_DOWN;
    m_keyIdToKeyNameMap[6]  = KeyNamesPB::keynames::ACTION;

    m_keyIdToKeyNameMap[8]  = KeyNamesPB::keynames::PRESET_1;
    m_keyIdToKeyNameMap[9]  = KeyNamesPB::keynames::PRESET_2;
    m_keyIdToKeyNameMap[10] = KeyNamesPB::keynames::PRESET_3;
    m_keyIdToKeyNameMap[11] = KeyNamesPB::keynames::PRESET_4;
    m_keyIdToKeyNameMap[12] = KeyNamesPB::keynames::PRESET_5;
    m_keyIdToKeyNameMap[13] = KeyNamesPB::keynames::PRESET_6;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::CustomProcessKeyEvent
///
/// @param  const LpmServiceMessages::IpcKeyInformation_t& keyEvent
///
/// @return This method returns a false value to allow further processing of the key event in the
///         base ProductKeyInputManager class. Future custom processing of key events can be put
///         here inside this method.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::CustomProcessKeyEvent( const LpmServiceMessages::IpcKeyInformation_t&
                                                          keyEvent )
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
