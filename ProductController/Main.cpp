/*
  Eddie application.

  This is the main file of Eddie application process.
 */

#include "Daemon.h"
#include "DPrint.h"
#include "version.h"

static DPrint s_logger( "EddieApp" );

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
    BOSE_INFO( s_logger, "Version: " VERSION_MAJOR "." VERSION_MINOR "."
               VERSION_PATCH "." VERSION_BUILD_NUMBER " "
               VERSION_BUILD_DEVELOPER "@" VERSION_BUILD_MACHINE " "
               VERSION_BUILD_TIME );
}

void EddieApplication::PrepareToDie()
{
    BOSE_INFO( s_logger, __func__ );
}

std::unique_ptr<Daemon> Daemon::Factory()
{
    return std::unique_ptr<Daemon> { new EddieApplication{} };
}

