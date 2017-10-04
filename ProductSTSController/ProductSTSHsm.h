////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSHsm.h
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Hsm.h"
#include "ProductSTSStateTop.h"
#include "Flatten.h"

class ProductSTSHsm : public CHsm<ProductSTSStateTop>
{
public:
    ProductSTSHsm( const std::string& name, NotifyTargetTaskIF *pTask ) :
        CHsm<ProductSTSStateTop>( name, pTask ) {}

    virtual ~ProductSTSHsm() {}

    template<typename... Ts>
    void Handle( bool ( ProductSTSStateTop::*f )( Ts... ), Ts... ts )
    {
        BOSE_DEBUG( m_hsmLogger, "%s: %s(%s)\n",
                    m_name.c_str(),
                    get_function_name( f, *( GetCurrentState() ) ).c_str(),
                    Flatten::FlattenText( ts... ).c_str() );
        CHsm<ProductSTSStateTop>::Handle<Ts...>( f, ts... );
    }
};
