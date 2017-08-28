#pragma once
#include <iostream>
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "DeviceManager.pb.h"

namespace ProductApp {
class ProductController;

class DeviceManager {
public:
    DeviceManager (NotifyTargetTaskIF* task, ProductController & m_controller);
    ~DeviceManager ();
    
    //////////////////////////////////////////////////////////////////////////////// 
    /// @name   GetDeviceInfo ()
    /// @brief  Returns DeviceInfo proto with device specific information
    /// @return DeviceInfo protobuf
    //////////////////////////////////////////////////////////////////////////////// 
    ::DeviceManager::Protobuf::DeviceInfo GetDeviceInfo ();

    //////////////////////////////////////////////////////////////////////////////// 
    /// @name   SetDeviceName ()
    /// @brief  Writes device name in persisten memory. This function is also called
    //          when no device name is assigned (in case of OOB of factory reset)
    /// @return true if DeviceName is set successfully 
    //////////////////////////////////////////////////////////////////////////////// 
    bool SetDeviceName (std::string devName);
private:
    ::DeviceManager::Protobuf::DeviceName CreateDefaultDeviceName ();

private:
    //Product controller task
    NotifyTargetTaskIF* m_task;

    //product controller reference 
    ProductController& m_productController;

    ProtoPersistenceIF::ProtoPersistencePtr     m_deviceNamePersist = nullptr;

    ::DeviceManager::Protobuf::DeviceName       m_deviceName;
};
}//namespace ProductApp
