#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "DPrint.h"
#include "LpmClientFactory.h"
#include "APTaskFactory.h"
#include "SystemUtils.h"

static DPrint s_logger{ "ReadLpmVersion" };

int main()
{
    int retVal = 1;
    std::string blobVersion = "";
    std::string psocVersion = "";
    std::string bootloaderVersion = "";
    std::string userappVersion = "";
    std::string lightbarVersion = "";
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
    /* Wait for all */
    timeout = 30;
    while( ( blobVersion.length() == 0
             || userappVersion.length() == 0
             || bootloaderVersion.length() == 0
             || psocVersion.length() == 0
             || lightbarVersion.length() == 0
           ) && timeout > 0 )
    {
        sleep( 1 );
        timeout--;
    }
    std::cout << "Blob Version: " <<  blobVersion << "\r\n";
    std::cout << "User App Version: " << userappVersion << "\r\n";
    std::cout << "Bootloader Version: " << bootloaderVersion << "\r\n";
    std::cout << "PSOC Version: " << psocVersion << "\r\n";
    std::cout << "Lightbar Version: " << lightbarVersion << "\r\n";

    return retVal;
}
