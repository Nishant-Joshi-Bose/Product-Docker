#pragma once
#include <iostream>
#include "NotifyTargetTaskIF.h"
#include "DeviceInfo.pb.h"

namespace ProductApp {
class ProductController;

class DeviceManager {
public:
    DeviceManager (NotifyTargetTaskIF* task, ProductController & m_controller);
    ~DeviceManager ();

    DeviceInfoPb::DeviceInfo getDeviceInfo ();

private:
    ///Product controller task
    NotifyTargetTaskIF* m_task;

    ///product controller reference 
    ProductController& m_productController;
    
};
}//namespace ProductApp
