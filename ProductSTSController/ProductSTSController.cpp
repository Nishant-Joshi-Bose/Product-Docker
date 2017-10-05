////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSController.cpp
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSController.h"
#include "ProductSTSAccount.h"
#include "APTaskFactory.h"
#include "BreakThread.h"
#include "NetworkPortDefines.h"
#include "ProtoToMarkup.h"
#include "STSProtocolVersion.h"

////////////////////////////////////////////////////////////////////////
ProductSTSController::ProductSTSController() :
    m_task( IL::CreateTask( "ProductSTSController" ) ),
    m_logger( "ProductSTSController" ),
    m_stsService( "ProductSTSService", m_task, m_delegate ),
    m_delegate( m_logger, *this, m_stsService, m_accounts )
{
}

////////////////////////////////////////////////////////////////////////
void ProductSTSController::Initialize( const std::vector<SourceDescriptor>& sources,
                                       const Callback<void> & HandleSTSInitWasComplete,
                                       const Callback<ProductSTS::ProductSourceSlot> & HandleSelectSourceSlot,
                                       const std::string& ip, const int port )
{
    m_HandleSTSInitWasComplete = HandleSTSInitWasComplete;
    m_HandleSelectSourceSlot = HandleSelectSourceSlot;
    ProductSTSAccount::SetProductSTSController( this );
    BOSE_INFO( m_logger, "%s %d sources", __func__ , sources.size() );
    for( const auto& source : sources )
    {
        ProductSTSAccount* account = new ProductSTSAccount{ source.sourceName, source.sourceID, source.enabled };
        BOSE_INFO( m_logger, "%s account name=%s, source=%d, %sabled", __func__ ,
                   account->GetSourceName().c_str(), account->GetSourceID(),
                   account->GetEnabled() ? "en" : "dis" );
        m_accounts[source.sourceID] = account;
    }
    m_stsService.Initialize( ip, port );
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::UpdateSources( const std::vector<SourceDescriptor>& sources )
{
    for( const auto& source : sources )
    {
        SetSourceEnabled( source.sourceID, source.enabled );
    }
}

//////////////////////////////////////////////////////////////////
ProductSTSController::ProductSTSServiceDelegate::ProductSTSServiceDelegate( DPrint& logger,
        ProductSTSController& controller,
        STSService& stsService,
        const std::map<ProductSTS::ProductSourceSlot, ProductSTSAccount*>& accounts ) :
    m_logger( logger ),
    m_controller( controller ),
    m_stsService( stsService ),
    m_accounts( accounts )
{
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::Connected()
{
    BOSE_INFO( m_logger, "ProductSTSServiceDelegate::Connected" );
    m_accountsToSetUp.reserve( m_accounts.size() );
    for( auto account = m_accounts.rbegin(); account != m_accounts.rend(); ++account ) // start at the end, see PAELLA-8627
    {
        m_accountsToSetUp.push_back( account->second );
    }
    m_stsService.SendProtocolVersionRequest();
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::HandleProtocolVersionResponse(
    const STS::ProtocolVersionResponse& res )
{
    BOSE_INFO( m_logger, "ProductSTSServiceDelegate::HandleProtocolVersionResponse(%d.%d)",
               res.major(), res.minor() );
    STS::RegisterServiceRequest req;
    req.set_source( SHELBY_SOURCE::PRODUCT );
    m_stsService.SendRegisterServiceRequest( req );
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::HandleServiceInfoRequest(
    const STS::Void&, const uint32_t seq )
{
    STS::ServiceInfoResponse res;
    res.set_isavailable( false );
    m_stsService.SendServiceInfoResponse( res, seq );
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::HandleRegisterServiceResponse( const STS::Void& )
{
    BOSE_INFO( m_logger, "ProductSTSServiceDelegate::HandleRegisterServiceResponse()" );

    RegisterNextAccount();
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::HandleCreateAccountProxyResponse(
    const STS::CreateAccountProxyResponse & res )
{
    BOSE_INFO( m_logger, "ProductSTSServiceDelegate::HandleCreateAccountProxyResponse" );

    ProductSTSAccount* accountToInitialize = m_accountsToSetUp.back();
    accountToInitialize->Initialize( res.port() );
    m_accountsToSetUp.pop_back();

    RegisterNextAccount();
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::HandleFlexUIRequest( const STS::Void&,
        const uint32_t seq )
{
    STS::FlexUIResponse res;
    m_stsService.SendFlexUIResponse( res, seq );
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::RegisterNextAccount()
{
    if( !m_accountsToSetUp.empty() )
    {
        ProductSTSAccount* accountToCreate = m_accountsToSetUp.back();
        BOSE_INFO( m_logger, "%s creating %s", __func__, accountToCreate->GetSourceName().c_str() );
        m_stsService.SendCreateAccountProxyRequest( accountToCreate->GetSourceName(), false );
        // The account creation will trigger a call to HandleCreateAccountProxyResponse() which will
        // pop the already-created account from m_accountsToSetUp and call here again to either
        // create the next account or detemine that we're done
    }
    else
    {
        m_accountsToSetUp.shrink_to_fit();

        m_controller.m_HandleSTSInitWasComplete();
    }
}

//////////////////////////////////////////////////////////////////
void ProductSTSController::ProductSTSServiceDelegate::HandleIntrospectRequest( const STS::Void& req, uint32_t seq )
{
    m_stsService.SendIntrospectResponse( ProtoToMarkup::ToXML( m_controller.Introspect() ) , seq );
}

//////////////////////////////////////////////////////////////////
bool ProductSTSController::SourceHasAnAccount( ProductSTS::ProductSourceSlot source ) const
{
    return nullptr != FindAccountForSource( source );
}


//////////////////////////////////////////////////////////////////
void ProductSTSController::SetSourceEnabled( ProductSTS::ProductSourceSlot source, bool enabled )
{
    ProductSTSAccount* account = FindAccountForSource( source );
    if( account == nullptr )
    {
        BOSE_ERROR( m_logger,  "ProductSTSController::SetSourceEnabled(%d) source not found", source );
        return;
    }
    account->SetEnabled( enabled );
}


//////////////////////////////////////////////////////////////////
const ProductSTSAccount* ProductSTSController::FindAccountForSource( ProductSTS::ProductSourceSlot source ) const
{
    const auto account = m_accounts.find( source );
    if( account != m_accounts.cend() )
    {
        return account->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////
ProductSTS::ProductSourceSlot ProductSTSController::FindSourceIDForSourceName( const std::string& sourceName ) const
{
    for( const auto& account : m_accounts )
    {
        if( sourceName == account.second->GetSourceName() )
        {
            return account.second->GetSourceID();
        }
    }

    return ProductSTS::SLOT_INVALID;
}

//////////////////////////////////////////////////////////////
void ProductSTSController::HandleSelectSourceSlot( ProductSTS::ProductSourceSlot source  )
{
    m_HandleSelectSourceSlot( source );
}


/////////////////////////////////////////////////////////////////////////////////////////
ProductSTSIntrospect::ProductSTSIntrospectResponse ProductSTSController::Introspect()
{
    ProductSTSIntrospect::ProductSTSIntrospectResponse introspectResponse;

    introspectResponse.set_systemname( m_stsService.GetName() );

    for( const auto& account : m_accounts )
    {
        introspectResponse.add_accounts( account.second->GetSourceName() );

    }

    return introspectResponse;
}

