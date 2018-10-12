////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTopAiQ.h
/// @brief    ProductSTSStateTop for sourcces that do not play audio
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ControlIntegrationSTSStateTop.h"

class ProductSTSStateTopAiQ : public ControlIntegrationSTSStateTop
{
public:
    ProductSTSStateTopAiQ( ControlIntegrationSTSHsm& hsm,
                           CHsmState *pSuperState,
                           ControlIntegrationSTSAccount& account );
    ~ProductSTSStateTopAiQ() override {}

protected:
    const std::string& GetURL( ) const override;

private:
    static std::string      s_aiqAudioURL;
};
