/*
  Professor SoundTouch product.

  This is the main file of the current Product Professor project.
 */

#include <signal.h>

#include "DPrint.h"

static DPrint s_logger( "Professor" );

//using namespace Professor;

void SignalHandler( int signal )
{
    exit( signal );
}

int main( int argc, char** argv )
{
    DPrint::Initialize( "Professor" );
    signal( SIGINT,  SignalHandler );
    signal( SIGTERM, SignalHandler );

    //ControllerTask controllerTask;

    //ControllerITC controllerItc( controllerTask );

    //SoundTouchsdkInterface.Start();

    BOSE_DEBUG( s_logger, "Professor product v0.1" );

    //    for( ;; );

    return 0;
}
