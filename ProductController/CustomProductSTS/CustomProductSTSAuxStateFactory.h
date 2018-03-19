////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSAuxStateFactory.h
/// @brief    Creates objects of type CustomProductSTSStateTopAux
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSCommonStateFactory.h"

class CustomProductSTSAuxStateFactory : public ProductSTSCommonStateFactory
{
public:
    CustomProductSTSAuxStateFactory( ) {};
    ~CustomProductSTSAuxStateFactory( ) override {};
    ProductSTSStateTop* Create( ProductSTSHsm& hsm,
                                CHsmState *pSuperState,
                                ProductSTSAccount& account ) override;
};
