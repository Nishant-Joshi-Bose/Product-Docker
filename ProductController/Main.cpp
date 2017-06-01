/*
  Eddie SoundTouch product.

  This is the main file of the current Product Eddie project.
 */

#include <signal.h>

#include "DPrint.h"

static DPrint s_logger( "Eddie" );

//using namespace Eddie;

void SignalHandler( int signal )
{
    exit( signal );
}

int main( int argc, char** argv )
{
    DPrint::Initialize( );
    signal( SIGINT,  SignalHandler );
    signal( SIGTERM, SignalHandler );

    //ControllerTask controllerTask;

    //ControllerITC controllerItc( controllerTask );

    //SoundTouchsdkInterface.Start();

    BOSE_DEBUG( s_logger, "Eddie product v0.1" );

    //    for( ;; );

    return 0;
}
