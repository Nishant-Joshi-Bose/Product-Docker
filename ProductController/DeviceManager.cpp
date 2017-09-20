#include "DPrint.h"
#include "ProtoPersistenceFactory.h"
#include "DeviceManager.h"
#include "ProductController.h"
#include "ProtoToMarkup.h"

static DPrint s_logger( "DeviceManager" );
namespace ProductApp
{

const std::string g_ProductPersistenceDir = "product-persistence/";

DeviceManager :: DeviceManager( NotifyTargetTaskIF* task,
                                ProductController & m_controller ):
    m_task( task ),
    m_productController( m_controller )
{
    m_deviceNamePersist = ProtoPersistenceFactory :: Create( "DeviceName", g_ProductPersistenceDir );
}
DeviceManager :: ~DeviceManager()
{
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
::DeviceManager::Protobuf::DeviceName DeviceManager :: CreateDefaultDeviceName()
{
    ::DeviceManager::Protobuf::DeviceName devName;
    return devName;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
::DeviceManager::Protobuf::DeviceInfo DeviceManager :: GetDeviceInfo()
{
    BOSE_INFO( s_logger, __func__ );
    ::DeviceManager::Protobuf::DeviceInfo devInfo;
    std::string s;
    //
    //proto may not exist.
    try
    {
        s = m_deviceNamePersist->Load();
        ProtoToMarkup::FromJson( s, &m_deviceName );
        BOSE_INFO( s_logger, "device name %s", s.c_str() );
    }
    catch( ... )
    {
        //this can happen for OOB or factory reset. In that case, give a default name
        s = "Bose SoundTouch 1234";
        SetDeviceName( s );

    }

    devInfo.set_name( s );

    //Name should be from persisten data and if don't find the name then
    //default to "Bose SoundTouch mac-id"
    //devInfo.set_name        ("Bose SoundTouch xxxx");
    //TODO - Below parameters will be available through HSP APIs
    devInfo.set_type( "SoundTouch 05" );
    devInfo.set_variant( "Eddie" );
    devInfo.set_guid( "xxxx-xxxx-xx" );
    devInfo.set_serialnumber( "1234567890" );
    devInfo.set_moduletype( "Riviera" );
    devInfo.set_countrycode( "US" );
    devInfo.set_regioncode( "US" );

    return devInfo;
}

bool DeviceManager :: SetDeviceName( std::string name )
{
    m_deviceName.set_devname( name );
    m_deviceNamePersist->Remove();
    m_deviceNamePersist->Store( ProtoToMarkup::ToJson( m_deviceName, false ) );
    return true;

}
} //namespace ProductApp
