/*
  Eddie application.

  This is the main file of Eddie application process.
 */

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdexcept>
#include "DPrint.h"
#include "BoseVersion.h"
#include "SystemUtils.h"
#include "EddieProductController.h"

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

    BOSE_INFO( s_logger, "Version: " VERSION_STRING_LONG );

    ::signal( SIGPIPE, SIG_IGN );

    ProductApp::EddieProductController eddieProductController;
    eddieProductController.Initialize();
    while( true )
        ::pause();
}
catch( std::exception const& exc )
{
    std::clog << SystemUtils::GetEXE() << ": Caught top level exception: "
              << exc.what() << std::endl;
    _exit( 1 );
}
