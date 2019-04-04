/*
  Product controller daemon's main().
 */

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdexcept>
#include "DPrint.h"
#include "BoseVersion.h"
#include "SystemUtils.h"
#include "CustomProductController.h"

namespace
{

DPrint s_logger( "Main" );

} // namespace

int main( int argc, char** argv )
try
{
    DPrint::Initialize();
    SystemUtils::ThereCanBeOnlyOne();

    if( argc != 1 )
        BOSE_DIE( "unexpected argument: '" << argv[1] << '\'' );

    BOSE_INFO( s_logger, "Version: %s", VERSION_STRING_LONG );

    ::signal( SIGPIPE, SIG_IGN );

    ProductApp::CustomProductController customProductController;
    customProductController.Initialize();
    while( true )
        ::pause();
}
catch( std::exception const& exc )
{
    std::clog << SystemUtils::GetEXE() << ": Caught top level exception: "
              << exc.what() << std::endl;
    _exit( 1 );
}
