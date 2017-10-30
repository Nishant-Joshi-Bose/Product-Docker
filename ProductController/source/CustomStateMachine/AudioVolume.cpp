////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      AudioVolume.h
///
/// @brief     This file implements a class for managing audio volume.
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "AudioService.pb.h"
#include "AudioVolume.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume::AudioVolume
///
/// @param frontDoor& FrontDoor client interface
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
AudioVolume<VolumeType>::AudioVolume( std::shared_ptr<FrontDoorClientIF> frontDoor ) 
    : frontDoorClient(frontDoor) 
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume::operator=
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator=(VolumeType v)
{
    current = v;
    current = std::min(maximum, current);
    current = std::max(minimum, current);

    return current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume::operator+
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator+(VolumeType v)
{
    current = std::min(current + v, maximum);
    return current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume<VolumeType>::operator-
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator-(VolumeType v)
{
    current = std::max(current - v, minimum);
    return current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume<VolumeType>::operator++ (postfix)
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator++(VolumeType v)
{
    current = std::min(current + stepSize, maximum);
    return current;
}

//////////////////////////////////////////////////////////////////////////////////////////////////// ///
/// @name  AudioVolume<VolumeType>::operator-- (postfix)
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator--(VolumeType v)
{
    current = std::max(current - stepSize, minimum);
    return current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume<VolumeType>::operator++ (prefix)
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator++()
{
    VolumeType pre = current;
    current = std::min(current + stepSize, maximum);
    return pre;
}

//////////////////////////////////////////////////////////////////////////////////////////////////// ///
/// @name  AudioVolume<VolumeType>::operator-- (prefix)
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator--()
{
    VolumeType pre = current;
    current = std::max(current - stepSize, minimum);
    return pre;
}




////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
