/////////////////////////////////////////////////////////////////////////////////
/// @file  DataCollectionClientInterface.h
/// @brief This class is the Interface between Product Controller and
///        DataCollection
///////////////////////////////////////////////////////////////////////////////

#include "DataCollectionClientIF.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "EndPointsDefines.h"
#include "NotifyTargetTaskIF.h"
#include "FrontDoorClient.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "DeviceManager.pb.h"

class DataCollectionClientInterface
{
public:
    DataCollectionClientInterface( const std::shared_ptr<FrontDoorClientIF> &frontDoorClientIF,  const std::shared_ptr<DataCollectionClientIF>& dataCollectionPTR );
    ~DataCollectionClientInterface() { }

    void Subscribe();
    void ProcessSystemState( const DeviceManagerPb::DeviceState& ds );

private:

    void HandleNowPlayingRequest( const SoundTouchInterface::NowPlaying& nPb, const DeviceManagerPb::DeviceState& ds );
    void GetCallbackError( const FrontDoor::Error& );

    NotifyTargetTaskIF*                          m_dataCollectionClientInterfaceTask = nullptr;
    std::shared_ptr<FrontDoorClientIF>           m_frontDoorClientIF;
    std::shared_ptr<DataCollectionClientIF>      m_dataCollectionClient;
};
