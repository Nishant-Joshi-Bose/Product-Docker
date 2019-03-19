////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTopAiQ.h
/// @brief    ProductSTSStateTop for sourcces that do not play audio
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSStateTop.h"

class ProductSTSStateTopAiQ : public ProductSTSStateTop
{
public:
    ProductSTSStateTopAiQ( ProductSTSHsm& hsm,
                           CHsmState *pSuperState,
                           ProductSTSAccount& account );
    ~ProductSTSStateTopAiQ() override {}

protected:
    const std::string& GetURL( ) const override;

private:
    static std::string      s_aiqAudioURL;
};
