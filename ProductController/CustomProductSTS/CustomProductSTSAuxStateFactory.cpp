////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSAuxStateFactory.cpp
/// @brief    Creates objects of type CustomProductSTSStateTopAux
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductSTSAuxStateFactory.h"
#include "CustomProductSTSStateTopAux.h"

ProductSTSStateTop* CustomProductSTSAuxStateFactory::Create( ProductSTSHsm& hsm,
                                                             CHsmState *pSuperState,
                                                             ProductSTSAccount& account )
{
    return new CustomProductSTSStateTopAux{ hsm, pSuperState, account };
}
