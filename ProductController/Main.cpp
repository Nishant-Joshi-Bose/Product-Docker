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
#include "ProductController.h"

namespace
{

DPrint s_logger( "EddieApp" );

struct EddieApp
{
    void Initialize();
    ProductApp::ProductController m_ProductController;
};

void EddieApp::Initialize()
{
    BOSE_INFO( s_logger, __func__ );
}

} // namespace

int main( int argc, char** argv )
try
{
    DPrint::Initialize();
    SystemUtils::ThereCanBeOnlyOne();

    BOSE_INFO( s_logger, "Version: " VERSION_MAJOR "." VERSION_MINOR "."
               VERSION_PATCH "." VERSION_BUILD_NUMBER " "
               VERSION_BUILD_DEVELOPER "@" VERSION_BUILD_MACHINE " "
               VERSION_BUILD_TIME );

    ::signal( SIGPIPE, SIG_IGN );

    EddieApp app;
    app.Initialize();
    while( true )
        ::pause();
}
catch( std::exception const& exc )
{
    std::clog << SystemUtils::GetEXE() << ": Caught top level exception: "
              << exc.what() << std::endl;
    _exit( 1 );
}
