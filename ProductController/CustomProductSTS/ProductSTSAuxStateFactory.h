////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSAuxStateFactory.h
/// @brief    Creates objects of type ProductSTSStateTopAux
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSCommonStateFactory.h"

class ProductSTSAuxStateFactory : public ProductSTSCommonStateFactory
{
public:
    ProductSTSAuxStateFactory( ) {};
    ~ProductSTSAuxStateFactory( ) override {};
    ProductSTSStateTop* Create( ProductSTSHsm& hsm,
                                CHsmState *pSuperState,
                                ProductSTSAccount& account ) override;
};
