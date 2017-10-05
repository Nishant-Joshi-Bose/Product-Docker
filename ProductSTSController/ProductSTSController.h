////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSController.h
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "STSService.h"
#include <string>
#include "ProductSTSAccount.h"
#include "DPrint.h"
#include "ProductSTSIntrospect.pb.h"
#include "Callback.h"

class ProductSTSController
{
public:
    ProductSTSController();
    ~ProductSTSController() {}
    typedef struct
    {
        ProductSTS::ProductSourceSlot   sourceID;
        std::string                 sourceName;
        bool                        enabled;
    } SourceDescriptor;
    void Initialize( const std::vector<SourceDescriptor>& sources, const Callback<void> & HandleSTSInitWasComplete,
                     const Callback<ProductSTS::ProductSourceSlot> & HandleSelectSourceSlot,
                     const std::string& ip = "127.0.0.1", const int port = STS_SERVICE_PORT );
    void UpdateSources( const std::vector<SourceDescriptor>& sources );
    bool SourceHasAnAccount( ProductSTS::ProductSourceSlot source ) const;
    void SetSourceEnabled( ProductSTS::ProductSourceSlot source, bool enabled );

    const ProductSTSAccount* FindAccountForSource( ProductSTS::ProductSourceSlot source ) const;
    ProductSTS::ProductSourceSlot FindSourceIDForSourceName( const std::string& sourceName ) const;

    void HandleSelectSourceSlot( ProductSTS::ProductSourceSlot source );

private:
    ProductSTSController( const ProductSTSController & ) = delete;
    ProductSTSController &operator=( const ProductSTSController & ) = delete;

    ProductSTSAccount* FindAccountForSource( ProductSTS::ProductSourceSlot source )
    {
        // See http://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
        // After Effective C++, 3d ed by Scott Meyers
        return const_cast<ProductSTSAccount*>( static_cast<const ProductSTSController &>( *this ).FindAccountForSource( source ) );
    }
    ProductSTSIntrospect::ProductSTSIntrospectResponse Introspect();

private:
    NotifyTargetTaskIF* m_task;
    DPrint m_logger;
    STSService m_stsService;
    std::map<ProductSTS::ProductSourceSlot, ProductSTSAccount*> m_accounts;
    Callback<void> m_HandleSTSInitWasComplete;
    Callback<ProductSTS::ProductSourceSlot> m_HandleSelectSourceSlot;

    class ProductSTSServiceDelegate : public STSServiceDelegate
    {
    public:
        ProductSTSServiceDelegate( DPrint& logger,
                                   ProductSTSController& controller,
                                   STSService& stsService,
                                   const std::map<ProductSTS::ProductSourceSlot, ProductSTSAccount*>& accounts );

        /// @brief This function is called when service is connected to BoseApp
        void Connected() override;

        /// @brief This function is called when protocol version response is received
        /// @param req ProtocolVersionResponse proto
        void HandleProtocolVersionResponse( const STS::ProtocolVersionResponse& res ) override;

        /// @brief This function is called when server makes a service info request
        /// @param seq IPC sequence number - must be sent in the IPC response
        void HandleServiceInfoRequest( const STS::Void& req, uint32_t seq ) override;

        /// @brief This function is called when server makes a Flex UI request
        /// @param seq IPC sequence number - must be sent in the IPC response
        void HandleFlexUIRequest( const STS::Void& req, uint32_t seq ) override;

        /// @brief This function is called when server registers the service
        void HandleRegisterServiceResponse( const STS::Void& ) override;

        /// @brief This function is called when server responds to create account proxy request - success
        /// @param res CreateAccountProxyResponse proto
        void HandleCreateAccountProxyResponse( const STS::CreateAccountProxyResponse& ) override;

        /// @brief This function is called when server makes an introspect request
        /// @param seq IPC sequence number - must be sent in the IPC response
        void HandleIntrospectRequest( const STS::Void& req, uint32_t seq ) override;

    private:
        void RegisterNextAccount();
    private:
        DPrint& m_logger;
        ProductSTSController& m_controller;
        STSService& m_stsService;
        const std::map<ProductSTS::ProductSourceSlot, ProductSTSAccount*>& m_accounts;
        std::vector<ProductSTSAccount*> m_accountsToSetUp;
    } m_delegate;
};
