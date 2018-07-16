#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "DPrint.h"
#include "LpmClientFactory.h"
#include "APTaskFactory.h"
#include "SystemUtils.h"

namespace
{

DPrint s_logger{ "ReadLpmVersion" };

void TrimNulls( std::string& s )
{
    while( !s.empty() && s.back() == '\0' )
        s.pop_back();
}

}

int main()
{
    int retVal = 1;
    std::string blobVersion;
    std::string psocVersion;
    std::string bootloaderVersion;
    std::string userappVersion;
    std::string lightbarVersion;
    std::string dspVersion;
    std::string darrVersion;
    std::string f0Version;
    std::string cc2640Version;
    int timeout = 30;

    auto task = IL::CreateTask( "ReadLpmVersionTask" );

    /* connect */
    auto lpmClient = LpmClientFactory::Create( "ReadLpmVersionLpmClient", task );

    lpmClient->Connect( [&retVal, lpmClient]( bool connected ) mutable
    {
        if( !connected )
        {
            retVal = -1;
            return;
        }
        retVal = 0;
    } );
    while( retVal != 0 && timeout > 0 )
    {
        sleep( 1 );
        timeout--;
    }
    if( 0 != retVal )
    {
        BOSE_CRITICAL( s_logger, "LPMClient Cannot be connected" );
        return retVal;
    }
    BOSE_INFO( s_logger, "Requesting Version information" );

    /* Blob */
    lpmClient->GetBlobVersion( [&blobVersion]( IpcBlobVersion_t const & rsp ) mutable
    {
        /* Using cout to print the output intentionally */
        blobVersion = rsp.version();
    } );
    /* user app */
    IpcLpmComponentVerReq_t req{};
    req.set_lpmcomponenttype( LPM_USER_APP );
    lpmClient->GetLpmComponentVersion( req, [&userappVersion]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        userappVersion = rsp.componentversion();
    } );
    /* bootloader */
    req.set_lpmcomponenttype( LPM_BOOTLOADER );
    lpmClient->GetLpmComponentVersion( req, [&bootloaderVersion]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        bootloaderVersion = rsp.componentversion();
    } );
    /* psoc */
    req.set_lpmcomponenttype( PSOC_USER_APP );
    lpmClient->GetLpmComponentVersion( req, [&psocVersion]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        psocVersion = rsp.componentversion();
    } );
    /* lightbar */
    req.set_lpmcomponenttype( LIGHTBAR_ANIMATION_DB );
    lpmClient->GetLpmComponentVersion( req, [&lightbarVersion]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        lightbarVersion = rsp.componentversion();
    } );

    req.set_lpmcomponenttype( DSP );
    lpmClient->GetLpmComponentVersion( req, [&dspVersion]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        dspVersion = rsp.componentversion();
    } );

    req.set_lpmcomponenttype( DARR );
    lpmClient->GetLpmComponentVersion( req, [&darrVersion]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        darrVersion = rsp.componentversion();
    } );

    req.set_lpmcomponenttype( F0 );
    lpmClient->GetLpmComponentVersion( req, [&f0Version]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        f0Version = rsp.componentversion();
    } );

    req.set_lpmcomponenttype( CC2640 );
    lpmClient->GetLpmComponentVersion( req, [&cc2640Version]( IpcLpmComponentVerRsp_t const & rsp ) mutable
    {
        cc2640Version = rsp.componentversion();
    } );
    /* Wait for all */
    timeout = 30;
    while( ( blobVersion.length() == 0
             || userappVersion.length() == 0
             || bootloaderVersion.length() == 0
             || psocVersion.length() == 0
             || lightbarVersion.length() == 0
             || dspVersion.length() == 0
             || darrVersion.length() == 0
             || f0Version.length() == 0
             || cc2640Version.length() == 0
           ) && timeout > 0 )
    {
        sleep( 1 );
        timeout--;
    }

    TrimNulls( blobVersion );
    TrimNulls( userappVersion );
    TrimNulls( bootloaderVersion );
    TrimNulls( psocVersion );
    TrimNulls( lightbarVersion );
    TrimNulls( dspVersion );
    TrimNulls( darrVersion );
    TrimNulls( f0Version );
    TrimNulls( cc2640Version );

    std::cout << "Blob Version: " << blobVersion << "\r\n";
    std::cout << "User App Version: " << userappVersion << "\r\n";
    std::cout << "Bootloader Version: " << bootloaderVersion << "\r\n";
    std::cout << "PSOC Version: " << psocVersion << "\r\n";
    std::cout << "Lightbar Version: " << lightbarVersion << "\r\n";
    std::cout << "DSP Version: " << dspVersion << "\r\n";
    std::cout << "DARR Version: " << darrVersion << "\r\n";
    std::cout << "F0 Version: " << f0Version << "\r\n";
    std::cout << "CC2640 Version: " << cc2640Version << "\r\n";

    return retVal;
}
