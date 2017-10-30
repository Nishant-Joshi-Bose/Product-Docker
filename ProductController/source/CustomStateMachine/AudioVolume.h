////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      AudioVolume.h
///
/// @brief     This file defines a class for managing audio volume.
///
/// @author    Chris Houston
///
/// @date      10/29/2017
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
#include "Hsm.h"
#include "HsmState.h"
#include "ProductMessage.pb.h"
#include "FrontDoorClientIF.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class AudioVolume
///
/// @brief This class is used to manage audio volume.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType> class AudioVolume
{

public:

    AudioVolume( std::shared_ptr<FrontDoorClientIF> frontDoor );
    ~AudioVolume( ) {};


    // The following operators manipulate volume, subject to min/max constraints.
    // Operators that result in an assignment (=/++/--) update the volume via FrontDoor.
    VolumeType operator=(VolumeType v);
    VolumeType operator+(VolumeType v);
    VolumeType operator-(VolumeType v);
    VolumeType operator++(VolumeType v);
    VolumeType operator--(VolumeType v);
    VolumeType operator++();
    VolumeType operator--();

private:
    std::shared_ptr<FrontDoorClientIF>  frontDoorClient;

    // current volume
    VolumeType current;

    // these are the minimum and maximum values the volume can take on
    VolumeType minimum;
    VolumeType maximum;

    // amount that volume changes for increment/decrement
    VolumeType stepSize;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
