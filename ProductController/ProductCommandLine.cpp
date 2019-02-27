/*
 * The ProductCommandLine class implements the Product's CLI commands.
 */

#include <endian.h>
#include <iostream>
#include <fstream>

#include "ProductCommandLine.h"
#include "DPrint.h"
#include "DirUtils.h"
#include "SystemUtils.h"
#include "LpmClientFactory.h"
#include "CLICmdsKeys.h"

namespace
{

DPrint s_logger{ "ProductCommandLine" };

template <typename UINT>
bool ToInteger( std::string const& str, UINT& result )
{
    // Return true if 'str' is a well-formed string representation of an
    // integer whose value will fit in 'result'.
    try
    {
        size_t end = 0;
        auto v = std::stoull( str, &end );
        result = v;
        if( result != v )
            return false;           // truncation/overflow
        return end == str.size();
    }
    catch( std::invalid_argument const& )
    {
        result = 0;
        return false;
    }
}

} // namespace

namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::ProductCommandLine
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine::ProductCommandLine( CustomProductController& ProductController ):

    m_ProductController( ProductController )
{
    BOSE_INFO( s_logger, __func__ );
}

/*! \brief  This method initializes CliClientMT and calls to register commands.
*/
void ProductCommandLine::Run( )
{
    BOSE_INFO( s_logger, __func__ );

    RegisterCliCmds();
}

/* \brief  Register the CLI commands for the product controller.
*/
void ProductCommandLine::RegisterCliCmds()
{
    BOSE_INFO( s_logger, __func__ );

    auto callback = [this]( uint16_t                            cmdKey,
                            const std::list<std::string> &      argList,
                            AsyncCallback<std::string, int32_t> respCb,
                            int32_t                             transact_id )

    {
        HandleCliCmd( cmdKey, argList, respCb, transact_id );
    };

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product boot_status",
                                                                          "Output the status of the boot up state.",
                                                                          "product boot_status",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::BOOT_STATUS ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product mfgdata",
                                                                          "Show the manufacturing data.",
                                                                          "product mfgdata",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::MFGDATA ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product backlight",
                                                                          "Adjust the LCD back light level.",
                                                                          "backlight",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::BACKLIGHT ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product lightsensor",
                                                                          "Get the light sensor LUX value.",
                                                                          "light sensor",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::LIGHTSENSOR ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product amp",
                                                                          "Get/set amplifier state.",
                                                                          "product amp [on|off|mute|unmute]",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::AMP ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product lcd",
                                                                          "Manage the lcd hardware.",
                                                                          "product lcd",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::LCD ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product battery",
                                                                          "Get/set battery status.",
                                                                          "product battery",
                                                                          m_ProductController.GetTask(),
                                                                          callback,
                                                                          static_cast<int>( CLICmdsKeys::BATTERY ) );

    m_ProductController.m_Clock.RegisterCliCmds( m_ProductController.GetCommonCliClientMT() ) ;
}

/* \brief  Callback function for registered product controller CLI commands.
*/
void ProductCommandLine::HandleCliCmd( uint16_t                              cmdKey,
                                       const std::list< std::string >&       argList,
                                       AsyncCallback< std::string, int32_t > respCb,
                                       int32_t                               transact_id )
{
    BOSE_INFO( s_logger, __func__ );

    std::string response( "Success" );

    for( const auto& v : argList )
    {
        BOSE_INFO( s_logger, "%s :", v.c_str() );
    }

    switch( static_cast< CLICmdsKeys >( cmdKey ) )
    {
    case CLICmdsKeys::BOOT_STATUS:
        HandleBootStatus( argList, response );
        break;

    case CLICmdsKeys::MFGDATA:
        HandleMfgdata( argList, response );
        break;

    case CLICmdsKeys::BACKLIGHT:
        HandleBacklight( argList, response );
        break;

    case CLICmdsKeys::LIGHTSENSOR:
        HandleLightsensor( argList, response );
        break;

    case CLICmdsKeys::AMP:
        HandleAmp( argList, response );
        break;

    case CLICmdsKeys::LCD:
        HandleLcd( argList, response );
        break;

    case CLICmdsKeys::BATTERY:
        HandleBattery( argList, response );
        break;

    default:
        response = "Command not found";
        break;
    }

    respCb( response, transact_id );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Start of callback functions to process each registered product controller CLI command.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/* \brief  This command outputs the boot up status of the device.
*/
void ProductCommandLine::HandleBootStatus( const std::list<std::string>& argList,
                                           std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    std::string CapsInitialized( m_ProductController.IsCAPSReady( )                 ? "true" : "false" );
    std::string LpmConnected( m_ProductController.IsLpmReady( )                     ? "true" : "false" );
    std::string audioPathConnected( m_ProductController.IsAudioPathReady( )         ? "true" : "false" );
    std::string networkModuleReady( m_ProductController.IsNetworkModuleReady( )     ? "true" : "false" );
    std::string StsInitialized( m_ProductController.IsSTSReady( )                   ? "true" : "false" );
    std::string bluetoothInitialized( m_ProductController.IsBluetoothModuleReady( ) ? "true" : "false" );
    std::string UiConnected( m_ProductController.IsUiConnected( )                   ? "true" : "false" );
    std::string SassInitialized( m_ProductController.IsSassReady( )                 ? "true" : "false" );
    std::string SoftwareUpdateReady( m_ProductController.IsSoftwareUpdateReady( )   ? "true" : "false" );
    std::string VoiceInitialized( m_ProductController.IsVoiceModuleReady( )         ? "true" : "false" );

    response  = "------------- Product Controller Booting Status -------------\n";
    response += "\n";
    response += "CAPS Initialized      : ";
    response += CapsInitialized;
    response += "\n";
    response += "LPM Connected         : ";
    response += LpmConnected;
    response += "\n";
    response += "Audio Path Connected  : ";
    response += audioPathConnected;
    response += "\n";
    response += "Network Module Ready  : ";
    response += networkModuleReady;
    response += "\n";
    response += "STS Initialized       : ";
    response += StsInitialized;
    response += "\n";
    response += "Bluetooth Initialized : ";
    response += bluetoothInitialized;
    response += "\n";
    response += "UI Connected          : ";
    response += UiConnected;
    response += "\n";
    response += "Software Update Ready : ";
    response += SoftwareUpdateReady;
    response += "\n";
    response += "SASS Initialized      : ";
    response += SassInitialized;
    response += "\n";
    response += "Voice Initialized     : ";
    response += VoiceInitialized;
    response += "\n";
    response += "\n";

    if( m_ProductController.IsBooted( ) )
    {
        response += "The device has been successfully booted.";
    }
    else
    {
        response += "The device has not yet been booted.";
    }
}

/*! \brief  This command retrieves manufacturing data from file
*/
void ProductCommandLine::HandleMfgdata( const std::list<std::string>& argList,
                                        std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    if( !argList.empty() )
    {
        response = "Wrong usage";
        return;
    }
    auto f = SystemUtils::ReadFile( "/persist/mfg_data.json" );
    if( f )
    {
        response = *f;
    }
    else
    {
        response = "No manufacturing data";
    }
}

/*! \brief  This command sets the intensity of the backlight.
*/
void ProductCommandLine::HandleBacklight( const std::list<std::string>& argList,
                                          std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    if( m_ProductController.IsLpmReady() == false )
    {

        response = "lpmClient not connected";
        return;
    }

    if( argList.size() == 0 )
    {
        m_ProductController.GetLpmHardwareInterface()->GetLpmClient()->GetBackLight( [this]( IpcBackLight_t const & rsp )
        {
            std::ostringstream ss;
            ss << rsp.value() << "%";
            BOSE_LOG( INFO, ss.str() );
            m_ProductController.GetCommonCliClientMT().SendAsyncResponse( ss.str() );

        } );

        return;
    }// If there is no argument

    if( argList.size() == 1 )
    {
        uint8_t intensity = 0;
        auto&   arg       = argList.front();

        if( !ToInteger( arg, intensity ) )
        {
            response = "Malformed integer: " + arg;
            return;
        }

        if( intensity > 100 )
        {
            response = "back light intensity must between 0..100";
            return;
        }

        IpcBackLight_t backlight;
        backlight.set_value( intensity );
        m_ProductController.GetLpmHardwareInterface()->GetLpmClient()->SetBackLight( backlight );

        return;
    }// If there is 1 argument

    response = "usage: backlight [0..100]";
    return;
}

/*! \brief  This command returns the value of the lightsensor.
*/
void ProductCommandLine::HandleLightsensor( const std::list<std::string>& argList,
                                            std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    if( m_ProductController.IsLpmReady() == false )
    {

        response = "lpmClient not connected";
        return;
    }

    if( argList.size() == 0 )
    {
        m_ProductController.GetLpmHardwareInterface()->GetLpmClient()->GetLightSensor( [this]( IpcLightSensor_t const & rsp )
        {
            std::ostringstream ss;
            int                lux_decimal    = ( int )( be16toh( rsp.lux_decimal_value() ) );
            int                lux_fractional = ( int )( be16toh( rsp.lux_fractional_value() ) );

            ss << lux_decimal << "." << lux_fractional << " LUX";
            BOSE_LOG( INFO, ss.str() );
            m_ProductController.GetCommonCliClientMT().SendAsyncResponse( ss.str() );

        } );

        return;
    }// If there is no argument

    response = "usage: lightsensor";
    return;
}

/*! \brief  This command sets values for the amplifier state.
 */
void ProductCommandLine::HandleAmp( const std::list<std::string>& argList,
                                    std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    if( m_ProductController.IsLpmReady() == false )
    {

        response = "lpmClient not connected";
        return;
    }

    if( argList.empty() )
    {
        BOSE_LOG( INFO, "Get amplifier status..." );
        m_ProductController.GetLpmHardwareInterface()->GetLpmClient()->GetAmp( [this]( IpcAmp_t const & rsp )
        {
            std::ostringstream ss;
            ss << "amplifier on=" << rsp.on()
               << " mute=" << rsp.mute();

            auto const& msg = ss.str();

            BOSE_LOG( INFO, msg );
            m_ProductController.GetCommonCliClientMT().SendAsyncResponse( msg );
        } );
        response = "Sent request for amplifier status";
    }
    else
    {
        IpcAmp_t req;
        for( auto& arg : argList )
        {
            if( arg == "on" )
            {
                req.set_on( true );
                continue;
            }
            if( arg == "off" )
            {
                req.set_on( false );
                continue;
            }
            if( arg == "mute" )
            {
                req.set_mute( true );
                continue;
            }
            if( arg == "unmute" )
            {
                req.set_mute( false );
                continue;
            }
            response = "Expected on|off|mute|unmute got '" + arg + '\'';
            return;
        }
        m_ProductController.GetLpmHardwareInterface()->GetLpmClient()->SetAmp( req );
        response = "Sent request to set amplifier state";
    }
}

/*! \brief  This command turns LCD on and off.
 */
void ProductCommandLine::HandleLcd( const std::list<std::string>& argList,
                                    std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    const std::string str_usage                    = "usage";
    const std::string display_controller_file_name = "/sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/send_command";
    auto& arg                                      = ( argList.size() == 1 ) ? argList.front() : str_usage;

    if( DirUtils::DoesFileExist( display_controller_file_name ) == false )
    {
        response = "error: can't find file: " + display_controller_file_name + " - " + strerror( errno );
        BOSE_LOG( ERROR, response );
        return;
    }

    std::ofstream display_controller_stream( display_controller_file_name );

    if( display_controller_stream.is_open() == false )
    {
        response = "error: failed to open file: " + display_controller_file_name + " - " + strerror( errno );
        BOSE_LOG( ERROR, response );
        return;
    }

    if( arg == "off" )
    {
        display_controller_stream << "28"; // see ST7789VI_SPEC_V1.4.pdf
        response = "lcd turned on";
    }
    else if( arg == "on" )
    {
        display_controller_stream << "29"; // see ST7789VI_SPEC_V1.4.pdf
        response = "lcd turned on";
    }
    else
    {
        response = "usage: lcd [on | off]";
    }

    return;
}

/*! \brief  Implementation of the `battery` CLI command.
 */

void ProductCommandLine::HandleBattery( const std::list<std::string>& argList,
                                        std::string& response )
{
    BOSE_INFO( s_logger, __func__ );

    auto batteryStatus = m_ProductController.GetBatteryManager()->GetBatteryStatus();
    if( argList.empty() )
    {
        std::ostringstream ss;
        ss << "BM: Battery chargeStatus=" << batteryStatus.chargeStatus
           << " minutesToFull=" << batteryStatus.minutesToEmpty
           << " minutesToEmpty=" << batteryStatus.minutesToEmpty
           << " percent=" << batteryStatus.percent;

        auto const& msg = ss.str();
        BOSE_LOG( INFO, msg );
        m_ProductController.GetCommonCliClientMT().SendAsyncResponse( msg );
        response = "Retrieved battery status";
    }
    else if( argList.size() == 2 )
    {
        auto&   arg       = argList.front(); // [charge, percent, mfull, mempty]
        auto&   val       = argList.back(); //value we want to set the arg to
        uint32_t temp     = 0;

        //Convert val to integer
        if( !ToInteger( val, temp ) )
        {
            response = "Malformed integer: " + val;
            return;
        }

        auto batteryStatus = m_ProductController.GetBatteryManager()->GetBatteryStatus();

        response = "usage: battery [charge|percent|mfull|mempty] [int]";

        if( arg == "charge" )
        {
            batteryStatus.chargeStatus =  temp;
        }
        else if( arg == "percent" )
        {
            batteryStatus.percent = temp;
        }
        else if( arg == "mfull" )
        {
            batteryStatus.minutesToFull = temp;
        }
        else if( arg == "mempty" )
        {
            batteryStatus.minutesToEmpty = temp;
        }
        else
        {
            return;
        }
        m_ProductController.GetBatteryManager()->DebugSetBattery( batteryStatus );
        response = "Sent to client";
    }
    return;
}

} //namespace ProductApp
