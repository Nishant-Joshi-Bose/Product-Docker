////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductLpmHardwareInterface.h
///
/// @brief     This header file declares a CustomProductLpmHardwareInterface class that inherits the
///            base ProductLpmHardwareInterface for managing the hardware, which interfaces with the
///            Low Power Microprocessor or LPM on Riviera based boards.
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
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ProductLpmHardwareInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

class CustomProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductLpmHardwareInterface
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductLpmHardwareInterface : public ProductLpmHardwareInterface
{
public:

    ///
    /// CustomProductLpmHardwareInterface Constructor
    ///
    CustomProductLpmHardwareInterface( CustomProductController& ProductController );

    ///
    /// CustomProductLpmHardwareInterface Deconstructor
    ///
    ~CustomProductLpmHardwareInterface( ) override
    {

    }

    ///
    /// Add custom functions here that implements product specific hardware functionality.
    ///
};

} // namespace
