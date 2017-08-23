#include "DeviceManager.h"
#include "ProductController.h"

namespace ProductApp {
DeviceManager :: DeviceManager (NotifyTargetTaskIF* task,
                                ProductController & m_controller):
    m_task (task),
    m_productController (m_controller)
//DeviceManager :: DeviceManager (NotifyTargetTaskIF* task)
{
}
DeviceManager :: ~DeviceManager ()
{
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DeviceInfoPb::DeviceInfo DeviceManager :: getDeviceInfo ()
{
    DeviceInfoPb::DeviceInfo devInfo;

    devInfo.set_name        ("Bose SoundTouch xxxx");
    devInfo.set_type        ("SoundTouch 10");
    devInfo.set_variant     ("Eddie");

    ///TODO - GUID will be provided by HSP API
    devInfo.set_guid        ("xxxx-xxxx-xx");

    //TODO - Serial # will be provide by HSP API
    devInfo.set_serialnumber("1234567890");
    devInfo.set_moduletype  ("Riviera");
    devInfo.set_countrycode ("US");
    devInfo.set_regioncode  ("US");

    return devInfo;
}
} //namespace ProductApp
