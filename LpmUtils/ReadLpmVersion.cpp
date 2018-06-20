#include <iostream>
#include "DPrint.h"
#include "LpmClientFactory.h"
#include "APTaskFactory.h"
#include <semaphore.h>

static DPrint s_logger{ "ReadLpmVersion" };

#define OK( X ) do {                                                    \
        if( ( X ) == -1 )                                               \
            BOSE_DIE( "Failed at " << __LINE__ << ": " << strerror( errno ) ); \
    } while( 0 )

int main()
{
    timespec timeout;
    OK( clock_gettime( CLOCK_REALTIME, &timeout ) );
    timeout.tv_sec += 30;

    auto task = IL::CreateTask( "ReadLpmVersionTask" );
    auto c = LpmClientFactory::Create( "ReadLpmVersionLpmClient", task );

    sem_t count;
    OK( sem_init( &count, 0, 0 ) );

    c->Connect( [&count, c]( bool connected )
    {
        if( !connected )
            BOSE_DIE( "LPM client connect failed" );
        OK( sem_post( &count ) );
    } );
    OK( sem_timedwait( &count, &timeout ) );

    c->GetBlobVersion( [&count]( IpcBlobVersion_t const & rsp )
    {
        printf( "Blob Version: %s\n", rsp.version().c_str() );
        OK( sem_post( &count ) );
    } );
    OK( sem_timedwait( &count, &timeout ) );

    IpcLpmComponentVerReq_t req{};
    req.set_lpmcomponenttype( LPM_USER_APP );
    c->GetLpmComponentVersion( req, [&count]( IpcLpmComponentVerRsp_t const & rsp )
    {
        printf( "User App Version: %s\n", rsp.componentversion().c_str() );
        OK( sem_post( &count ) );
    } );
    OK( sem_timedwait( &count, &timeout ) );

    req.set_lpmcomponenttype( LPM_BOOTLOADER );
    c->GetLpmComponentVersion( req, [&count]( IpcLpmComponentVerRsp_t const & rsp )
    {
        printf( "Bootloader Version: %s\n", rsp.componentversion().c_str() );
        OK( sem_post( &count ) );
    } );
    OK( sem_timedwait( &count, &timeout ) );

    req.set_lpmcomponenttype( PSOC_USER_APP );
    c->GetLpmComponentVersion( req, [&count]( IpcLpmComponentVerRsp_t const & rsp )
    {
        printf( "PSOC Version: %s\n", rsp.componentversion().c_str() );
        OK( sem_post( &count ) );
    } );
    OK( sem_timedwait( &count, &timeout ) );

    req.set_lpmcomponenttype( LIGHTBAR_ANIMATION_DB );
    c->GetLpmComponentVersion( req, [&count]( IpcLpmComponentVerRsp_t const & rsp )
    {
        printf( "Lightbar Version: %s\n", rsp.componentversion().c_str() );
        OK( sem_post( &count ) );
    } );
    OK( sem_timedwait( &count, &timeout ) );
}
