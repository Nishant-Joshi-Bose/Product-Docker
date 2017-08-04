////////////////////////////////////////////////////////////////////////////////
/// @file   ProductController.h
/// @brief  Generic Product controller class for Riviera based product
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NotifyTargetTaskIF.h"
#include "FrontDoorClientInterface.h"
#include "ProductAppHsm.h"
#include "ProductAppStateTop.h"
#include "ProductAppStateBooting.h"
#include "ProductAppStateStdOp.h"
#include "ProductAppStateSetup.h"
#include "ProductAppStateStandby.h"

namespace ProductApp
{
class ProductAppHsm;
class ProductController
{
public:
    ProductController();
    virtual ~ProductController();
    inline NotifyTargetTaskIF* GetTask() const
    {
        return m_ProductControllerTask;
    }
    /// Handle requests from Front door client and hands it over to Product application HSM for further processing.
    void HandleFrontDoorRequest( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation );

private:
    /// Disable copies
    ProductController( const ProductController& ) = delete;
    ProductController& operator=( const ProductController& ) = delete;

    void RegisterCallbacks();

private:
    NotifyTargetTaskIF*         m_ProductControllerTask;
    ProductAppHsm               m_ProductAppHsm;
    FrontDoorClientInterface    m_FrontDoorClientInterface;

    ProductAppStateTop          m_ProductAppStateTop;
    ProductAppStateBooting      m_ProductAppStateBooting;
    ProductAppStateStdOp        m_ProductAppStateStdOp;
    ProductAppStateSetup        m_ProductAppStateSetup;
    ProductAppStateStandby      m_ProductAppStateStandby;
};
} // namespace ProductApp
