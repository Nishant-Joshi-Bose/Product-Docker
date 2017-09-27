////////////////////////////////////////////////////////////////////////////////
/// @file   HelloWorldProductController.h
/// @brief  HelloWorld Product controller class
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductController.h"
#include "ProductControllerHsm.h"
#include "ProductControllerStateTop.h"
#include "CustomProductControllerStateBooting.h"
#include "ProductControllerStateSetup.h"
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStateOn.h"
#include "ProductControllerStateIdle.h"
#include "CustomProductControllerStateSwUpdating.h"
#include "FrontDoorClientIF.h"
#include "LpmClientIF.h"
#include "KeyHandler.h"
#include "Language.pb.h"

namespace ProductApp
{
class HelloWorldProductController : public ProductController
{
public:
    HelloWorldProductController( std::string const& ProductName );
    virtual ~HelloWorldProductController();

private:
    /// Disable copies
    HelloWorldProductController( const HelloWorldProductController& ) = delete;
    HelloWorldProductController& operator=( const HelloWorldProductController& ) = delete;

    ProductControllerHsm                    m_ProductControllerHsm;

    ProductControllerStateTop               m_ProductControllerStateTop;
    CustomProductControllerStateBooting     m_CustomProductControllerStateBooting;
    ProductControllerStateSetup             m_ProductControllerStateSetup;
    ProductControllerStateNetworkStandby    m_ProductControllerStateNetworkStandby;
    ProductControllerStateOn                m_ProductControllerStateOn;
    ProductControllerStateIdle              m_ProductControllerStateIdle;
    CustomProductControllerStateSwUpdating  m_CustomProductControllerStateSwUpdating;

    std::shared_ptr<FrontDoorClientIF>      m_FrontDoorClientIF;

    LpmClientIF::LpmClientPtr               m_LpmClient;
    KeyHandlerUtil::KeyHandler              m_KeyHandler;

    void InitializeLpmClient();
    void HandleLPMReady();
    void RegisterLpmEvents();
    void RegisterKeyHandler();
    void HandleLpmKeyInformation( IpcKeyInformation_t keyInformation );
    static void KeyInformationCallBack( const int result, void *context );

public:
    void HandleGetLanguageRequest( const Callback<ProductPb::Language> &resp );

private:
    void RegisterEndPoints();
};
} // namespace ProductApp
