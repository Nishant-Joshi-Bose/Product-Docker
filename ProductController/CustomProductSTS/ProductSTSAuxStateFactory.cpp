////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSAuxStateFactory.cpp
/// @brief    Creates objects of type ProductSTSStateTopAux
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSAuxStateFactory.h"
#include "ProductSTSStateTopAux.h"

ProductSTSStateTop* ProductSTSAuxStateFactory::Create( ProductSTSHsm& hsm,
                                                       CHsmState *pSuperState,
                                                       ProductSTSAccount& account )
{
    return new ProductSTSStateTopAux{ hsm, pSuperState, account };
}
