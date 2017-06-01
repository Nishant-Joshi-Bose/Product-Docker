/*
  Eddie application.

  This is the main file of Eddie application process.
 */

#include "Daemon.h"
#include "DPrint.h"

static DPrint s_logger( "EddieApplication" );

//using namespace Eddie;


class EddieApplication : public Daemon
{

public:

    EddieApplication();
    void PrepareToLive() override;
    void PrepareToDie() override;

public:

    //ControllerTask m_controllerTask;
    //ControllerITC m_controllerItc;
    //EddieSoundTouchInterface m_SoundTouchInterface;
    //EddieCliClient m_eddieCliClient;
};

EddieApplication::EddieApplication()  : Daemon()
{
}

void EddieApplication::PrepareToLive()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieApplication::PrepareToDie()
{
    BOSE_INFO( s_logger, __func__ );
}

std::unique_ptr<Daemon> Daemon::Factory()
{
    return std::unique_ptr<Daemon> { new EddieApplication{} };
}

