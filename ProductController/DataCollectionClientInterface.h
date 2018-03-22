/////////////////////////////////////////////////////////////////////////////////
//@file    DataCollectionClientInterface.h
//@brief   This class is the Interface between Eddie and DataCollection
///////////////////////////////////////////////////////////////////////////////

#include "DataCollectionClientIF.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "EndPointsDefines.h"
#include "NotifyTargetTaskIF.h"
#include "FrontDoorClient.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "AudioBassLevel.pb.h"
#include "WebInterface/Balance.pb.h"
#include "DeviceManager.pb.h"
#include "SharedProto.pb.h"

class DataCollectionClientInterface
{
public:
    DataCollectionClientInterface( const std::shared_ptr<FrontDoorClientIF> &frontDoorClientIF );
    ~DataCollectionClientInterface() { }

    void Subscribe();
    void ProcessSystemState( const DeviceManagerPb::DeviceState& ds );
    void ProcessBassState( const  ProductPb::AudioBassLevel& abl );
    void ProcessBalanceState( const  WebInterface::balance& b );
    //void ProcessLanguage( const DeviceManagerPb::ProductSettings& psPb );

private:

    void HandleNowPlayingRequest( const SoundTouchInterface::NowPlaying& nPb, const DeviceManagerPb::DeviceState& ds );
    void GetCallbackError( const FrontDoor::Error& );

    std::shared_ptr<DataCollectionClientIF>      m_dataCollectionClient;
    NotifyTargetTaskIF*                          m_dataCollectionClientInterfaceTask = nullptr;
    std::shared_ptr<FrontDoorClientIF>           m_frontDoorClientIF;
};
