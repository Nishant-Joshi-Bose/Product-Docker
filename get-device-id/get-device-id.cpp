/*
This program prints, in json format, all the information needed to verify the device
ID of a given unit. It is used in manufacturing at the last verify station.

It grabs the public key from the trust zone and the rest of the
information is pulled out of mfg data.
*/

#include <fstream>
#include <cstddef>
#include <iostream>
#include <string>
#include <cstdlib>
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/obj_mac.h"
#include "openssl/ec.h"
#include "openssl/bio.h"
#include "openssl/md5.h"
#include "curl/curl.h"
#include "json/json.h"

#include "RivieraUtils.hpp"
using namespace Bose;
using namespace Bose::Riviera;

constexpr char MFG_DATA[] = "/persist/mfg_data.json";
constexpr char DEVICE_TYPE[] = "RIVIERA";

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

string getMfgData( string key )
{
    Json::Value mfgData;
    std::ifstream mfgDataRaw( MFG_DATA, std::ifstream::binary );
    mfgDataRaw >> mfgData;
    return mfgData.get( key, "" ).asString();
}

//This function extracts a public key from an EVP_PKEY object and
//converts it to PEM format.
string getPublicECKey( EVP_PKEY *keypair )
{
    OpenSSL::Bio bp_public( BIO_new( BIO_s_mem() ) );
    if( !bp_public || !PEM_write_bio_PUBKEY( bp_public.get(), keypair ) )
    {
        cout << "Error PEM_write_bio_PublicKey()" << endl;
        return string();
    }

    char const * str = nullptr;
    long len = BIO_get_mem_data( bp_public.get(), &str );
    if( len <= 0 || !str )
    {
        cout << "Error getPublicECKey() can not get memory" << endl;
        return string();
    }

    return string( str, len );
}

//Assuming that the system has already generated a key pair and stored
//it away, this function knows where is would be stored and retrieves
//it. It returns the public key in PEM format.
string getStoredPublicKey()
{
    string pubKey;
    //On Riviera we need to ask the hsp what the private key it. If it
    //returns an empty key then then the keys don't exist.
    OpenSSL::EvpPKey keypair;

    try
    {
        if( !( keypair = GetDeviceKeyPair( Bose::Riviera::HspSecInit() ) ) )
        {
            cout << "GetDeviceKeyPair() failed: Assuming no keys are stored in the trust zone" << endl;
        }
        else
        {
            //This converts the key to pem format.
            pubKey = getPublicECKey( keypair.get() );
        }
    }
    catch( OpenSSL::OpenSSLError const & err )
    {
        cout << "OpenSSL Riviera error: " << err.what() << endl;
        return "";
    }
    catch( std::exception const & err )
    {
        cout << "std::exception Riviera error: " << err.what() << endl;
        return "";
    }
    catch( ... )
    {
        cout << "unknown Riviera error" << endl;
        return "";
    }
    return pubKey;
}

int main( int argc, char** argv )
{
    string guid;
    string pubKey;
    string productType;
    Json::Value deviceIdJson;
    std::ofstream file_id;

    if( argc > 1 )
    {
        cout << argv[0] << ": Does not take command line arguments." << endl;
    }

    guid = getMfgData( "guid" );
    pubKey = getStoredPublicKey();
    productType = getMfgData( "productType" );

    if( guid.empty() )
    {
        guid = "No guid";
    }

    if( pubKey.empty() )
    {
        pubKey = "No pubKey";
    }
    else
    {
        //Remove the trailing '\n'
        if( pubKey.back() == '\n' )
            pubKey.pop_back();
    }

    if( productType.empty() )
    {
        productType = "No productType";
    }

    deviceIdJson["guid"] = guid;
    deviceIdJson["pubkey"] = pubKey;
    deviceIdJson["modelType"] = productType;
    deviceIdJson["deviceType"] = DEVICE_TYPE;

    //print to stdout
    cout << deviceIdJson << endl;

    //print to file
    file_id.open( "/tmp/deviceID.json" );
    if( file_id )
    {
        file_id << deviceIdJson;
        file_id.close();
    }
    else
    {
        cout << "Error: Could not open /tmp/deviceID.json." << endl;
        return 1;
    }
    return 0;
}
