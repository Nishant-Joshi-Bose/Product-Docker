////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductAdaptIQManager.cpp
///
/// @brief     This file implements audio volume management.
///
/// @author    Chris Houston
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "DPrint.h"
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductAdaptIQManager.h"

using namespace ProductPb;

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the AdaptIQManager
///
////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char FRONTDOOR_ADAPTIQ[]                              = "/adaptiq";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductAdaptIQManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param mainTask
///
/// @param ProductNotify
///
/// @param HardwareInterface
///
/// @return This method returns a pointer to a ProductAdaptIQManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductAdaptIQManager* ProductAdaptIQManager::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                           Callback< ProductMessage > ProductNotify,
                                                           ProductHardwareInterface*  HardwareInterface )
{
    static ProductAdaptIQManager* instance = nullptr;

    if( instance == nullptr )
    {
        instance = new ProductAdaptIQManager( mainTask, ProductNotify, HardwareInterface );
        instance->Initialize();
    }

    BOSE_DEBUG( s_logger, "The instance %p of the ProductAdaptIQManager was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::Initialize
///
/// @brief  This method performs one-time initialization of this instance.  This is a good place
///         to put things that you may have wanted to do in the constructor but that might depend on
///         the object being fully-initialized.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::Initialize( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductAdaptIQManager" );

    auto getFunc = [ this ]( Callback<AdaptIQStatus> resp )
    {
        AdaptIQStatus s;
        resp.Send( s );
    };
    AsyncCallback<Callback<AdaptIQStatus>> getCb( getFunc, m_mainTask );
    m_FrontDoorClient->RegisterGet( FRONTDOOR_ADAPTIQ, getFunc );

    auto putFunc = [ this ]( const AdaptIQStatus & status, Callback<AdaptIQStatus> resp )
    {
    };
    AsyncCallback<const AdaptIQStatus&, Callback<AdaptIQStatus>> putCb( putFunc, m_mainTask );
    m_FrontDoorClient->RegisterPut<AdaptIQStatus>( FRONTDOOR_ADAPTIQ, putFunc );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::ProductAdaptIQManager
///
/// @brief  This method is the ProductAdaptIQManager constructor, which is declared as being private to
///         ensure that only one instance of this class can be created through the class GetInstance
///         method.
///
/// @param mainTask
///
/// @param ProductNotify
///
/// @param HardwareInterface
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductAdaptIQManager::ProductAdaptIQManager( NotifyTargetTaskIF*        mainTask,
                                              Callback< ProductMessage > ProductNotify,
                                              ProductHardwareInterface*  HardwareInterface )
    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::Run
///
/// @brief  This method starts the main task for the ProductAdaptIQManager instance. The OnEntry method
///         for the ProductAdaptIQManager instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductAdaptIQManager::Run( )
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductAdaptIQManager::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::Stop( void )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
