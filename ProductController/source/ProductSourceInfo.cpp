////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSourceInfo.cpp
///
/// @brief     This file implements audio volume management.
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
#include "CustomProductLpmHardwareInterface.h"
#include "ProductSourceInfo.h"
#include "EndPointsError.pb.h"

using namespace ProductPb;

namespace
{
const std::string s_FrontDoorSources = "/system/sources";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the SourceInfo
///
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSourceInfo::ProductSourceInfo
///
/// @param ProductController
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSourceInfo::ProductSourceInfo( ProfessorProductController& ProductController ) :
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductNotify( ProductController.GetMessageHandler( ) )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSourceInfo::Run
///
/// @brief  This method starts the main task for the ProductSourceInfo class.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSourceInfo::Run( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductSourceInfo" );

    auto handleSources = [ this ]( const SoundTouchInterface::Sources & sources )
    {
        UpdateSources( sources );
    };

    auto handleSourcesFail = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "Error %d %d <%s> while retrieving source list",
                    error.code( ), error.subcode( ), error.message( ).c_str( ) );
    };

    auto handleSourcesReady = [ this, handleSources, handleSourcesFail ]( const std::list<std::string>& endPointList )
    {
        if( endPointList.empty( ) )
        {
            BOSE_ERROR( s_logger, "Endpoint not ready" );
            return;
        }

        BOSE_INFO( s_logger, "Registering for %s",  s_FrontDoorSources.c_str() );
        m_FrontDoorClient->SendGet<SoundTouchInterface::Sources, EndPointsError::Error>( s_FrontDoorSources, handleSources, handleSourcesFail );
        m_FrontDoorClient->RegisterNotification<SoundTouchInterface::Sources>( s_FrontDoorSources, handleSources );
    };

    std::list<std::string> endPointList = { s_FrontDoorSources };
    m_FrontDoorClient->RegisterEndpointsOfInterest( endPointList, handleSourcesReady, {} );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSourceInfo::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSourceInfo::Stop( void )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSourceInfo::UpdateSources
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSourceInfo::UpdateSources( const SoundTouchInterface::Sources& sources )
{
    BOSE_INFO( s_logger, "%s got source update %s", __func__, ProtoToMarkup::ToJson( sources ).c_str() );
    m_sources = sources;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSourceInfo::UpdateSources
///
/// @param  item Currently-selected content item
///
/// @return BOptional containing source information for the source specified in the supplied content item
///
////////////////////////////////////////////////////////////////////////////////////////////////////
BOptional<SoundTouchInterface::Sources::SourceItem> ProductSourceInfo::FindSource( const SoundTouchInterface::ContentItem& item )
{
    if( ( not item.has_sourceaccount() ) or ( not item.has_source() ) )
    {
        return {};
    }

    for( auto i = 0 ; i < m_sources.sources_size(); i++ )
    {
        auto source = m_sources.sources( i );

        if( ( not source.has_sourceaccountname() ) or ( not source.has_sourcename() ) )
        {
            continue;
        }

        if( ( source.sourceaccountname() == item.sourceaccount() ) and ( source.sourcename() == item.source() ) )
        {
            return source;
        }
    }

    return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
