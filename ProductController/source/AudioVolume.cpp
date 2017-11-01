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
#include "AsyncCallback.h"
#include "Utilities.h"
#include "ProductMessage.pb.h"
#include "ProductController.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume::AudioVolume
///
/// @param frontDoor& FrontDoor client interface
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
AudioVolume<VolumeType>::AudioVolume( Callback<VolumeType> notifyChange ) :
    m_Minimum( DEFAULT_MINIMUM_VOLUME ),
    m_Maximum( DEFAULT_MAXIMUM_VOLUME ),
    m_Current( DEFAULT_CURRENT_VOLUME ),
    // m_Previous != m_Current to force initial update
    m_Previous( m_Current + 1 ),
    m_StepSize( DEFAULT_STEPSIZE ),
    notifyChangeCb( notifyChange )
{
    Notify();
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
VolumeType AudioVolume<VolumeType>::operator=( VolumeType v )
{
    m_Current = v;
    m_Current = std::min( m_Maximum, m_Current );
    m_Current = std::max( m_Minimum, m_Current );
    Notify();

    return m_Current;
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
VolumeType AudioVolume<VolumeType>::operator+( VolumeType v )
{
    m_Current = std::min( m_Current + v, m_Maximum );
    Notify();
    return m_Current;
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
VolumeType AudioVolume<VolumeType>::operator-( VolumeType v )
{
    m_Current = std::max( m_Current - v, m_Minimum );
    Notify();
    return m_Current;
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
VolumeType AudioVolume<VolumeType>::operator++( VolumeType v )
{
    m_Current = std::min( m_Current + m_StepSize, m_Maximum );
    Notify();
    return m_Current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume<VolumeType>::operator-- (postfix)
///
/// @param v new volume value
///
/// @return new volume value, subject to min/max constraints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
VolumeType AudioVolume<VolumeType>::operator--( VolumeType v )
{
    m_Current = std::max( m_Current - m_StepSize, m_Minimum );
    Notify();
    return m_Current;
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
    VolumeType pre = m_Current;
    m_Current = std::min( m_Current + m_StepSize, m_Maximum );
    Notify();
    return pre;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
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
    VolumeType pre = m_Current;
    m_Current = std::max( m_Current - m_StepSize, m_Minimum );
    Notify();
    return pre;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  AudioVolume<VolumeType>::Notify
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename VolumeType>
void AudioVolume<VolumeType>::Notify()
{
    if( m_Current != m_Previous )
    {
        notifyChangeCb( m_Current );
        m_Previous = m_Current;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
