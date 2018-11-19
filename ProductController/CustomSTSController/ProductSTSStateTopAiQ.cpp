////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTopAiQ.cpp
/// @brief    ProductSTSStateTop for sources that do not play audio
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSStateTopAiQ.h"

std::string ProductSTSStateTopAiQ::s_aiqAudioURL{ "adaptiq://" };

//////////////////////////////////////////////////////////////
ProductSTSStateTopAiQ::ProductSTSStateTopAiQ( ControlIntegrationSTSHsm& hsm,
                                              CHsmState *pSuperState,
                                              ControlIntegrationSTSAccount& account ) :
    ControlIntegrationSTSStateTop( hsm, pSuperState, account )
{
}

////////////////////////////////////////////////////////
///
/// The this class's version returns the URL for AiQ audio processing per PGC-1541
///
////////////////////////////////////////////////////////
const std::string& ProductSTSStateTopAiQ::GetURL( ) const
{
    return s_aiqAudioURL;
}
