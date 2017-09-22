#pragma once
#include "AlertsSystemClient.h"


class CAlertOutput
{
public:
    CAlertOutput( CAlertsSystemClient *aClient )
    {
        alertClient = aClient;
        alertClient->registerObserver( this );
    }
    virtual void onActiveAlert() = 0;
    virtual void onAlertAcknowledged() = 0;
private:
    CAlertsSystemClient *alertClient;

};
