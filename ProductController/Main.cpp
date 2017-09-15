/*
  HelloWorld application.

  This is the main file of HelloWorld application process.
 */

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <stdexcept>

#include "DPrint.h"
#include "BoseVersion.h"
#include "SystemUtils.h"
#include "HelloWorldProductController.h"

namespace
{

DPrint s_logger( "Professor - hellow world PC" );

struct HelloWorldApp
{
    void Initialize();
};

void HelloWorldApp::Initialize()
{
    BOSE_INFO( s_logger, __func__ );
}

} // namespace

int main( int argc, char** argv )
try
{
    if( argc > 1 )
    {
		BOSE_DIE( "unexpected argument: '" << argv[1] << '\'' );
    }

    DPrint::Initialize();
    SystemUtils::ThereCanBeOnlyOne();

 	BOSE_INFO( s_logger, "Version: " VERSION_MAJOR "." VERSION_MINOR "."
               VERSION_PATCH "." VERSION_BUILD_NUMBER " "
               VERSION_BUILD_DEVELOPER "@" VERSION_BUILD_MACHINE " "
               VERSION_BUILD_TIME );

    ::signal( SIGPIPE, SIG_IGN );

    HelloWorldApp app;
    app.Initialize();
    ProductApp::HelloWorldProductController m_HelloWorldProductController( "helloWorld" );
    while( true )
        ::pause();
}
catch( std::exception const& exc )
{
    std::clog << SystemUtils::GetEXE() << ": Caught top level exception: "
              << exc.what() << std::endl;
    _exit( 1 );
}
