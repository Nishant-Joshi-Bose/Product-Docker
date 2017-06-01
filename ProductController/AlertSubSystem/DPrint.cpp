////////////////////////////////////////////////////////////////////////////////
/// @file            DPrint.cpp
/// @brief           Logging.
///
/// Copyright 2011 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <netdb.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "DPrint.h"
#include "Panics.h"

using std::vector;
using std::map;
using std::string;
using std::list;
using Strings = vector<string>;

static bool s_logDateTime = false;

namespace
{

string GetEXE()
{
    return "";
}

const char* const levelNames[] =
{
    "CRITICAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TESTAUTO",
    "VERBOSE",
    "INSANE",
};

// Send to syslogd.
struct SysLog
{
    void Initialize( const std::string& identity );
    void vprintf( DPrint::Level priority, const char* facilityName,
                  const char* format, va_list args );

    /*!
     * Convert DPrint::Level to a syslog priority.
     *
     * @param level The DPrint level.
     *
     * @return A syslog(3) log level LOG_INFO, ...
     */
    static int priority( DPrint::Level level );

    SysLog();

    // Not copyable.
    SysLog( const SysLog& ) = delete;
    SysLog& operator=( const SysLog& ) = delete;

    /*! The socket descriptor for sending to syslogd, or -1. */
    int m_socket;

    /*! Protect modifications of m_socket.  It's not necessary to protect use
        of the socket, only changing the value of m_socket. */
   // CThreadMutex m_mutex;

    /*! Label to prefix to all log messages.  If not empty, then it also
        contains a trailing separator, e.g., ": ". */
    string m_identity;

    /*!
      Open the socket and connect to syslogd.

      Has no effect if the socket is already open.

      @return true if the socket is open, false if not.
    */
    bool open();

    /*!
      Close the socket to syslogd.

      Has no effect if the socket is not open.
    */
    void close();

    void Prefix( std::ostream& os,
                 DPrint::Level level,
                 char const* func,
                 string const& facility );
    void Put( std::ostringstream& ss );
};

SysLog& GetSysLog()
{
    static SysLog only;
    return only;
}

SysLog::SysLog()
    : m_socket( -1 ),
      //m_mutex(),
      m_identity()
{}

void SysLog::Initialize( const std::string& identity )
{
    std::ostringstream ss;
    ss << identity << '[' << getpid() << "]: ";
    m_identity = ss.str();
}

bool SysLog::open()
{
    if( m_socket != -1 )
        return true; // ok to send
    //CThreadMutexLocker lock( &m_mutex );
    if( m_socket != -1 )
        return true; // ok to send
    sockaddr_un a;
    memset( &a, 0, sizeof( a ) );
    a.sun_family = AF_UNIX;
    static char const socket_name[] = "/dev/log";
    static_assert( sizeof( a.sun_path ) >= sizeof( socket_name ), "oops" );
    memcpy( a.sun_path, socket_name, sizeof( socket_name ) );
    m_socket = socket( AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0 );
    if( m_socket == -1 )
    {
//        std::clog << "socket: " << ERR << std::endl;
        return false; // not ok to send
    }
    static int failed = 0;      // throttle error messages
    if( connect( m_socket, reinterpret_cast<sockaddr*>( &a ),
                 sizeof( a ) ) == -1 )
    {
//        if( failed == 0 )
 //           std::clog << "connect: " << ERR << std::endl;
        ++failed;
//        if( ::close( m_socket ) == -1 )
 //           std::clog << "close " << m_socket << ": " << ERR << std::endl;
        m_socket = -1;
        return false; // not ok to send
    }
    if( failed != 0 )
    {
//        std::clog << "Connected after dropping " << failed
//                  << " log message" << ( failed == 1 ? "" : "s" )
//                  << HERE << std::endl;
        failed = 0;
    }
    return true; // ok to send
}

void SysLog::close()
{
    if( m_socket == -1 )
        return;
//    CThreadMutexLocker lock( &m_mutex );
    if( m_socket == -1 )
        return;
//    if( ::close( m_socket ) == -1 )
 //       std::clog << "close " << m_socket << ": " << ERR << std::endl;
    m_socket = -1;
}

int SysLog::priority( DPrint::Level level )
{
    switch( level )
    {
    case DPrint::CRITICAL:
        return LOG_CRIT;
    case DPrint::ERROR:
        return LOG_ERR;
    case DPrint::WARNING:
        return LOG_WARNING;
    case DPrint::INFO:
        return LOG_INFO;
    case DPrint::DEBUG:
    case DPrint::VERBOSE:
    case DPrint::INSANE:
        return LOG_DEBUG;
    default:
        return LOG_ALERT;
    }
}

void SysLog::vprintf( DPrint::Level level, const char* facilityName,
                      const char* format, va_list args )
{
    iovec iovs[2];
    iovec *iovp = iovs;

    /*
      The "<%d>" prefix is explained in syslog(3):

       Every text line in a message has  its  own  loglevel.   This  level  is
       DEFAULT_MESSAGE_LOGLEVEL  - 1 (6) unless the line starts with <d> where
       d is a digit in the range 1-7, in which case the level is d.  The  con-
       ventional  meaning  of  the  loglevel is defined in <linux/kernel.h> as
       follows:

       #define KERN_EMERG    "<0>"  // system is unusable
       #define KERN_ALERT    "<1>"  // action must be taken immediately
       #define KERN_CRIT     "<2>"  // critical conditions
       #define KERN_ERR      "<3>"  // error conditions
       #define KERN_WARNING  "<4>"  // warning conditions
       #define KERN_NOTICE   "<5>"  // normal but significant condition
       #define KERN_INFO     "<6>"  // informational
       #define KERN_DEBUG    "<7>"  // debug-level messages

      What the man page doesn't explain is that you can OR-in the log facility
      (LOG_KERN, LOG_USER, ...).  The default is LOG_KERN.
    */
    char buf1[128];
    int len = snprintf( buf1, sizeof( buf1 ), "<%d>%s[(%06ld):%s:%s]",
                        priority( level ) | LOG_LOCAL0,
                        m_identity.c_str(), gettid(),
                        facilityName, levelNames[level] );
    if( len > 0 )
    {
        if( size_t( len ) >= sizeof( buf1 ) )
            /* buf1 wasn't big enough.  This shouldn't ever happen unless the
               m_identity and/or facilityName strings were foolishly long. */
            len = sizeof( buf1 ) - 1;
        iovp->iov_base = buf1;
        iovp->iov_len = len;
        ++iovp;
    }

    char buf2[1024];
    len = vsnprintf( buf2, sizeof( buf2 ), format, args );
    if( len > 0 )
    {
        if( size_t( len ) >= sizeof( buf2 ) )
        {
            /* buf2 wasn't big enough.  Mark the message as having been
               truncated. */
            buf2[ sizeof( buf2 ) - 2 ] = '$';
            len = sizeof( buf2 ) - 1;
        }
        iovp->iov_base = buf2;
        iovp->iov_len = len;
        ++iovp;
    }

    msghdr msg;
    memset( &msg, 0, sizeof( msg ) );
    msg.msg_iov = iovs;
    msg.msg_iovlen = iovp - iovs;

    if( m_socket == -1 && !open() )
        return;

    while( sendmsg( m_socket, &msg, 0 ) == -1 )
    {
        if( errno != EINTR )
        {
//            std::clog << "sendmsg " << m_socket << ": " << ERR << std::endl;
            close();
            break;
        }
    }
}

void SysLog::Prefix( std::ostream& os,
                     DPrint::Level level,
                     char const* func,
                     string const& facilityName )
{
    os << '<' << priority( level ) << '>'
       << m_identity
       << "[(" << std::setfill( '0' ) << std::setw( 6 ) << gettid()
       << "):" << facilityName << ':' << levelNames[level] << ']'
       << func << ": ";
}

void SysLog::Put( std::ostringstream& ss )
{
    auto const& msg = ss.str();
    if( m_socket == -1 && !open() )
        return;
    size_t len = msg.size();
    constexpr size_t maxLen = 1024; // Avoid EMSGSIZE "Message too long"
    if( len > maxLen )
        len = maxLen;
    while( send( m_socket, msg.data(), len, 0 ) == -1 )
    {
        if( errno != EINTR )
        {
//            std::clog << "send " << m_socket << ": " << ERR << std::endl;
            close();
            break;
        }
    }
}

struct Timestamp
{
    const char* generate();

private:

    char buf[ 32 ];
};

const char* Timestamp::generate()
{
    if( s_logDateTime )
    {
        // Log string "month/day/year hour/min/sec"
        // Example "11/20/15 08:02:10"
        struct tm timeInfo;
        memset( &timeInfo, 0, sizeof( timeInfo ) );

        time_t now = time( nullptr );
        if( localtime_r( &now, &timeInfo ) != nullptr )
        {
            // This doesn't adjust for daylight savings time.
            snprintf( buf, sizeof( buf ), "%02d/%02d/%02d %02d:%02d:%02d",
                      timeInfo.tm_mon, timeInfo.tm_mday, timeInfo.tm_year % 100,
                      timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec );
        }
        else
        {
            snprintf( buf, sizeof( buf ), "date-time unavailable" );
        }
    }
    else
    {
        // Log seconds and fractional seconds
        timespec t;
        clock_gettime( CLOCK_MONOTONIC, &t );
        unsigned s = t.tv_sec;
        unsigned ms = t.tv_nsec / ( 1000 * 1000 );
        snprintf( buf, sizeof( buf ), "%06u.%03u", s, ms );
    }
    return buf;
}

std::string lower( std::string s )
{
    std::transform( s.begin(), s.end(), s.begin(), tolower );
    return s;
}

}

////////////////////////////////////////////////////////////////////////////////
/// The Debug Print Logger Implementation class
////////////////////////////////////////////////////////////////////////////////
class DPrintImpl
{
public:
    enum LogOutputLocation
    {
        STDOUT,
        SYSLOG,
        USB
    };

    static DPrintImpl* Instance( );
    int RegisterDPrintClient( const string& facility );
    void RegisterDPrintClient( int index );
    void UnregisterDPrintClient( int index );

    void Log( DPrint::Level level, int index,
              const char* format, va_list args );

    void LogIfRequired( DPrint::Level level, int index,
                        const char* format, va_list args );

    bool SetGlobalLogLevel( DPrint::Level level );
    bool SetFacilityEnabled( const string &facility, bool enabled );
    void SetEnabled( bool bEnabled );
    bool SetFacilityLogLevel( const string& facility, DPrint::Level level );
    list<string> GetCurrentLoggingStatus( );

    void SetLogOutputLocation( LogOutputLocation logOutputLocation );
    void LoadConfigFile( const char* confFileName );
    void LoadConfigFile( std::istream&, const char* confFileName );
    void LoadConfigFile();

    /*!
     * Check if logging is enabled.
     *
     * @param level The log level (CRITICAL, ERROR, ...)
     *
     * @param facilityIndex The facility index as selected
     * by RegisterDPrintClient.
     *
     * return True if logging is enabled, false otherwise.
     */
    bool IsLog( DPrint::Level level, int facilityIndex );

    DPrintImpl( const DPrintImpl& ) = delete;
    DPrintImpl& operator=( const DPrintImpl& ) = delete;

    void Prefix( std::ostream& os,
                 DPrint::Level level,
                 int facilityIndex );
    void Prefix( std::ostream& os,
                 DPrint::Level level,
                 char const* func,
                 int facilityIndex );
    void Put( std::ostringstream& ss );

    void Put( std::ostringstream& ss, std::ostream& os );

private:
    DPrintImpl( );
    ~DPrintImpl( );
    bool CheckEnabledPatterns( const string& facility );
    DPrint::Level CheckLevelPatterns( const string& facility );
    static bool MatchFacility( const string& pattern, const string& facility );
    void ProcessConfig( const char* file, int line, const std::string&
                        facility, const std::string& level );
    bool ProcessConfigSet( const char* file, int line, Strings& args );
    void ProcessConfigCommand( const char*, int, Strings& );

    /* Convert the given string to a DPrint::Level value.
       Return -1 if the string isn't a valid level. */
    static int StringToLevel( const std::string& );

    static constexpr size_t maxFacilities = 1024;

    Strings m_FacilityNames;

    /*!
     * The number of times the facility has been registered.  Indexed by
     * facility index as selected by RegisterDPrintClient.
     */
    int m_FacilityRefCount[ maxFacilities ];

    /*!
     * True if the facility is enabled.  Indexed by facility index as selected
     * by RegisterDPrintClient.
     *
     * This data member is a legacy array (instead of std::vector<bool>, for
     * example) so that we know it won't resize at runtime and thus we don't
     * need to protect access with a mutex.
     */
    bool m_FacilityEnabled[ maxFacilities ];

    /*!
     * Log level per facility.  Indexed by facility index as selected by
     * RegisterDPrintClient.
     *
     * This data member is a legacy array (instead of
     * std::vector<DPrint::Level>, for example) so that we know it won't
     * resize at runtime and thus we don't need to protect access with a
     * mutex.
     */
    DPrint::Level m_FacilityLogLevel[ maxFacilities ];

    map<string, int> m_FacilityTable;   // a map of facility names and indices into the above vectors

    LogOutputLocation m_logOutputLocation;
    std::ofstream m_logFile;            // When m_logOutputLocation is USB

    bool m_enabledByDefault;            // This is used to determine if a dprint logger for a new facility
    // that is created after startup should default to enabled or not.
    map<string, bool> m_facilityPatterns; // Stores patterns that were previously specified
    DPrint::Level m_defaultLevel;       // This is used to keep track of the default dprint level for a new facility
    map<string, DPrint::Level> m_levelPatterns; // Stores patterns that were previously specified

    //CThreadMutex m_singletonSem;

    void LogToFile( DPrint::Level level, int facilityIndex, const char* format,
                    va_list args, std::ostream& os );

public:

    /* The application's name to be included in log messages.
       See DPrint::Initialize. */
    std::string m_appName;

    /*
      Test Automation Logging.
      This is legacy deprecated vestigial stuff.
      Don't use it.
    */
public:
    void LogForTestAutomation( int facilityIndex, const char* format, va_list args );
};

DPrintImpl::DPrintImpl( ) :
    m_logOutputLocation( SYSLOG ),
    m_enabledByDefault( true ),
    m_defaultLevel( DPrint::INFO )
{
}

DPrintImpl::~DPrintImpl()
{
    m_logFile.close();
}

inline DPrintImpl* DPrintImpl::Instance( )
{
    static DPrintImpl only;
    return &only;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets whether to send debug print logging to stdout or syslog
/// @param printToSyslog true to print to syslog, false for stdout
/// @return None
////////////////////////////////////////////////////////////////////////////////
void DPrintImpl::SetLogOutputLocation( LogOutputLocation logOutputLocation )
{
    //CThreadMutexLocker lock( &m_singletonSem );
    m_logOutputLocation = logOutputLocation;

    // If the output location is being set to USB then open the file in
    // preparation for log output being written to it
    if( logOutputLocation == USB )
    {
        timeval now;
        gettimeofday( &now, nullptr );
        auto timeStruct = gmtime( &now.tv_sec );
        std::stringstream logFilename;
        logFilename << "/media/sda1/" << GetEXE();
        if( timeStruct )
        {
            logFilename << "-" << timeStruct->tm_year + 1900 << "-"
                        << std::setw( 2 ) << std::setfill( '0' ) << timeStruct->tm_mon + 1 << "-"
                        << std::setw( 2 ) << std::setfill( '0' ) << timeStruct->tm_mday << "_"
                        << std::setw( 2 ) << std::setfill( '0' ) << timeStruct->tm_hour << "-"
                        << std::setw( 2 ) << std::setfill( '0' ) << timeStruct->tm_min << "-"
                        << std::setw( 2 ) << std::setfill( '0' ) << timeStruct->tm_sec << "."
                        << std::setw( 6 ) << std::setfill( '0' ) << now.tv_usec
                        << ".log";
        }
        else
        {
            logFilename << ".log";
        }
        m_logFile.open( logFilename.str(), std::ofstream::out | std::ofstream::app );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Registers a system facility with the debug print logger
/// @param facility the name of the facility
/// @param index the returned index into the facility table (needed for later use)
////////////////////////////////////////////////////////////////////////////////
int DPrintImpl::RegisterDPrintClient( const string &facility )
{
    //CThreadMutexLocker lock( &m_singletonSem );
    auto cur = m_FacilityTable.find( facility );
    if( cur != m_FacilityTable.end() )
    {
        /* If it already exists then return it's index and increment the
           reference count. */
        auto index = cur->second;
        ++m_FacilityRefCount[ index ];
        return index;
    }

    /* facility index 0 means we're probably using a static DPrint object
       whose constructor hasn't been run yet.  This can happen due to static
       initialization ordering problems.  See SHELBY-35386. */
    if( m_FacilityTable.empty() )
    {
        string unknown{ "*UNKNOWN*" };
        constexpr auto index = 0;
        m_FacilityNames.push_back( unknown );
        m_FacilityRefCount[index] = 1;
        m_FacilityEnabled[index] = true;
        m_FacilityLogLevel[index] = DPrint::INSANE;
        m_FacilityTable[unknown] = index;
    }

    size_t i;
    for( i = 0;; ++i )
    {
        if( i == m_FacilityNames.size() )
        {
            PANIC_ASSERT( i < maxFacilities,
                          "You need to increase DPrintImpl::maxFacilities" );
            m_FacilityNames.push_back( facility );

#if 0 /* 1 to record info about how big maxFacilities should be */
            {
                std::ostringstream ss;
                ss << "echo >>/tmp/" << GetEXE() << ".fac "
                   << i << " '" << facility << "'\n";
                system( ss.str().c_str() );
            }
#endif
            break;
        }
        auto& name = m_FacilityNames[ i ];
        if( name.empty() )
        {
            name = facility;
            break;
        }
    }

    // This facility may match a previously enabled or disabled pattern
    m_FacilityRefCount[ i ] = 1;
    m_FacilityEnabled[ i ] = CheckEnabledPatterns( facility );
    m_FacilityLogLevel[ i ] = CheckLevelPatterns( facility );
    m_FacilityTable[ facility ] = i;
    return i;
}

void DPrintImpl::RegisterDPrintClient( int index )
{
    //CThreadMutexLocker lock( &m_singletonSem );
    PANIC_ASSERT( size_t( index ) < m_FacilityNames.size(),
                  "Invalid index" );
    ++m_FacilityRefCount[ index ];
}

void DPrintImpl::UnregisterDPrintClient( int index )
{
    //CThreadMutexLocker lock( &m_singletonSem );

    if( size_t( index ) >= m_FacilityNames.size() )
        /* This would be unusual.  Perhaps RegisterDPrintClient threw
           an exception. */
        return;

    if( --m_FacilityRefCount[ index ] > 0 )
        return;

    /* facility index 0 means we're probably using a static DPrint object
       whose constructor hasn't been run yet.  This can happen due to static
       initialization ordering problems.  See SHELBY-35386. */
    if( index == 0 )
        return;

    auto& name = m_FacilityNames[ index ];
    m_FacilityTable.erase( name );
    name.clear();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Checks to see if this facility should be enabled by default or not
/// based on any previously entered patterns.
/// @param facility the name of the facility
/// @return whether the facility should be enabled
////////////////////////////////////////////////////////////////////////////////
bool DPrintImpl::CheckEnabledPatterns( const string& facility )
{
    for( auto const& pat : m_facilityPatterns )
    {
        if( MatchFacility( pat.first, facility ) )
        {
            return pat.second;
        }
    }

    // didn't match
    return m_enabledByDefault;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Checks to see what level this facility should be based on any
/// previously entered patterns.
/// @param facility the name of the facility
/// @return what level the facility should be set to
////////////////////////////////////////////////////////////////////////////////
DPrint::Level DPrintImpl::CheckLevelPatterns( const string& facility )
{
    for( auto const& pat : m_levelPatterns )
    {
        if( MatchFacility( pat.first, facility ) )
        {
            return pat.second;
        }
    }

    // didn't match
    return m_defaultLevel;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Compares the prefix pattern with the facility case insensitively
/// @param pattern - the pattern to use in the comparison
/// @param facility - the name of the facility
/// @return whether they match or not
////////////////////////////////////////////////////////////////////////////////
bool DPrintImpl::MatchFacility( const string& pattern, const string& facility )
{
    return ( lower( pattern.substr( 0, pattern.size() - 1 ) ) ==
             lower( facility.substr( 0, pattern.size() - 1 ) ) );
}

bool DPrintImpl::IsLog( DPrint::Level level, int facilityIndex )
{
    PANIC_ASSERT( size_t( facilityIndex ) < maxFacilities,
                  "Invalid facility index" );
    LoadConfigFile();

    /* facility index 0 means we're probably using a static DPrint object
       whose constructor hasn't been run yet.  This can happen due to static
       initialization ordering problems.  See SHELBY-35386. */
    if( facilityIndex == 0 )
        return true;

    if( not m_FacilityEnabled[ facilityIndex ] )
        return false;
    if( level <= m_FacilityLogLevel[ facilityIndex ] )
        return true;
    return false;
}

void DPrintImpl::LogToFile( DPrint::Level level,
                            int facilityIndex,
                            const char* format,
                            va_list args,
                            std::ostream& os )
{
    if( not os.good() )
        return;

    char buf[1024];
    int len = vsnprintf( buf, sizeof( buf ), format, args );
    if( len == -1 )
        // format string was invalid
        len = snprintf( buf, sizeof( buf ), "*OOPS*" );
    if( size_t( len ) >= sizeof( buf ) )
        len = sizeof( buf ) - 1;

    /* The trailing newline in the log message is optional.
       If it's present, then delete it so we don't get two newlines. */
    if( len > 0 && buf[len - 1] == '\n' )
        buf[len - 1] = '\0';

    //CThreadMutexLocker lock( &m_singletonSem );
    Prefix( os, level, facilityIndex );
    os << m_appName << buf << std::endl;
}

void DPrintImpl::Put( std::ostringstream& ss, std::ostream& os )
{
    if( not os.good() )
        return;
    auto const& msg = ss.str();
    //CThreadMutexLocker lock( &m_singletonSem );
    os << msg;
    if( msg.empty() or msg.back() != '\n' )
        os << '\n';
    os << std::flush;
}

void DPrintImpl::Log( DPrint::Level level,
                      int facilityIndex,
                      const char* format,
                      va_list args )
{
    switch( m_logOutputLocation )
    {
    case SYSLOG:
        GetSysLog().vprintf( level, m_FacilityNames[facilityIndex].c_str(),
                             format, args );
        break;

    case USB:
        LogToFile( level, facilityIndex, format, args, m_logFile );
        break;

    case STDOUT:
        LogToFile( level, facilityIndex, format, args, std::cout );
        break;

    default:
        std::clog << "Invalid m_logOutputLocation="
                  << m_logOutputLocation
                  << std::endl;
        break;
    }
}

void DPrintImpl::Prefix( std::ostream& os,
                         DPrint::Level level,
                         int facilityIndex )
{
    os << '[' << Timestamp().generate() << " ("
       << std::setfill( '0' ) << std::setw( 6 ) << gettid()
       << "):" << m_FacilityNames[facilityIndex]
       << ':' << levelNames[level]
       << ']';
}

void DPrintImpl::Prefix( std::ostream& os,
                         DPrint::Level level,
                         char const* func,
                         int facilityIndex )
{
    switch( m_logOutputLocation )
    {
    case SYSLOG:
        GetSysLog().Prefix( os, level, func, m_FacilityNames[facilityIndex] );
        break;

    case USB:
    case STDOUT:
        Prefix( os, level, facilityIndex );
        os << m_appName << func << ": ";
        break;

    default:
        std::clog << "Invalid m_logOutputLocation="
                  << m_logOutputLocation
                  << std::endl;
        break;
    }
}

void DPrintImpl::Put( std::ostringstream& ss )
{
    switch( m_logOutputLocation )
    {
    case SYSLOG:
        GetSysLog().Put( ss );
        break;

    case USB:
        Put( ss, m_logFile );
        break;

    case STDOUT:
        Put( ss, std::cout );
        break;

    default:
        std::clog << "Invalid m_logOutputLocation="
                  << m_logOutputLocation
                  << std::endl;
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Logs the string provided if the log level of the output is low enough
/// and if the facility doing the logging is enabled.
/// @param level the level of the debug print
/// @param facilityIndex index into the facility table of the facility doing the logging
/// @param format the printf style format of the string to be logged
/// @param args the printf style argument list
////////////////////////////////////////////////////////////////////////////////
inline
void DPrintImpl::LogIfRequired( DPrint::Level level,
                                int facilityIndex,
                                const char* format,
                                va_list args )
{
    if( IsLog( level, facilityIndex ) )
        Log( level, facilityIndex, format, args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the current log level for all facilities (present and future)
/// @param level the new log level
/// @return true if the log level was set, false otherwise (invalid level)
////////////////////////////////////////////////////////////////////////////////
bool DPrintImpl::SetGlobalLogLevel( DPrint::Level level )
{
    if( level < DPrint::CRITICAL or level > DPrint::INSANE )
        return false;

    //CThreadMutexLocker lock( &m_singletonSem );
    for( auto & fll : m_FacilityLogLevel )
        fll = level;
    m_defaultLevel = level;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the logging enabled flag of the given facility.
///
/// @param facility The facility to be enabled/disabled.  Can be a "wildcard"
/// pattern (ending with an asterisk like "foo*") in which case enable/disable
/// all registered facilities with names patching that pattern.  Can be "all"
/// to apply to all registered facilities.  This function registers the
/// facility name if not already registered and not "all" or a wildcard
/// pattern.
///
/// @param enabled How to set the facility (true=enabled, false=disabled)
///
/// @return true if the facility flag is set, false if facility is a wildcard
/// pattern that doesn't match any registered facilities.
////////////////////////////////////////////////////////////////////////////////
bool DPrintImpl::SetFacilityEnabled( const string& facility, bool enabled )
{
    if( facility.empty() )
        return false;

    if( facility == "all" )
    {
        SetEnabled( enabled );
        return true;
    }

    //CThreadMutexLocker lock( &m_singletonSem );
    if( facility.back() == '*' )
    {
        bool anyMatched = false;
        for( auto& f : m_FacilityTable )
        {
            if( MatchFacility( facility, f.first ) )
            {
                m_FacilityEnabled[f.second] = enabled;
                anyMatched = true;
            }
        }

        // Save off this pattern in case any future facilities are registered
        // that match it.
        m_facilityPatterns[ facility ] = enabled;
        return anyMatched;
    }

    auto cur = m_FacilityTable.find( facility );
    if( cur != m_FacilityTable.end() )
    {
        int index = cur->second;
        m_FacilityEnabled[index] = enabled;
        return true;
    }

    // The facility isn't registered yet, so register it now and set the
    // enabled/disabled flag.
    auto index = RegisterDPrintClient( facility );
    m_FacilityEnabled[index] = enabled;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the logging enabled flag of all facilities
///
/// @param enabled how to set the facility (true=enabled, false=disabled)
////////////////////////////////////////////////////////////////////////////////
void DPrintImpl::SetEnabled( bool bEnabled )
{
    //CThreadMutexLocker lock( &m_singletonSem );
    m_enabledByDefault = bEnabled;
    for( auto const& fac : m_FacilityTable )
    {
        m_FacilityEnabled[fac.second] = bEnabled;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Sets the logging level of the given facility,
///
/// @param facility The facility whose logging level is to be set.  Can be a
/// "wildcard" pattern (ending with an asterisk like "foo*") in which case
/// enable/disable all registered facilities with names patching that pattern.
/// Can be "all" to apply to all registered facilities.  This function
/// registers the facility name if not already registered and not "all" or a
/// wildcard pattern.
///
/// @param level Log level to set for the facility (one of DPrint::Level).
///
/// @return true if the facility flag is set, false if facility is a wildcard
/// pattern that doesn't match any registered facilities, false if the level
/// is invalid.
////////////////////////////////////////////////////////////////////////////////
bool DPrintImpl::SetFacilityLogLevel( const string& facility,
                                      DPrint::Level level )
{
    if( facility.empty() )
        return false;

    if( level < DPrint::CRITICAL || level > DPrint::INSANE )
    {
        return false;
    }

    if( facility == "all" )
    {
        return SetGlobalLogLevel( level );
    }

    //CThreadMutexLocker lock( &m_singletonSem );
    if( facility.back() == '*' )
    {
        bool anyMatched = false;
        for( auto& f : m_FacilityTable )
        {
            if( MatchFacility( facility, f.first ) )
            {
                m_FacilityLogLevel[f.second] = level;
                anyMatched = true;
            }
        }

        // Save off this pattern in case any future facilities are registered
        // that match it.
        m_levelPatterns[facility] = level;
        return anyMatched;
    }

    auto cur = m_FacilityTable.find( facility );
    if( cur != m_FacilityTable.end() )
    {
        int index = cur->second;
        m_FacilityLogLevel[index] = level;
        return true;
    }

    // The facility isn't registered yet, so register it now and set the
    // log level.
    auto index = RegisterDPrintClient( facility );
    m_FacilityLogLevel[index] = level;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Constructs a list of strings which together provide the current status
/// of the logger (i.e. current log level, list of registered facilities along
/// with their current enabled state).  Also provides a basic level of help by
/// displaying the log levels to choose from and a couple of small examples.
/// @param None
/// @return the list of strings containing all the info
////////////////////////////////////////////////////////////////////////////////
list<string> DPrintImpl::GetCurrentLoggingStatus( )
{
    //CThreadMutexLocker lock( &m_singletonSem );
    list<string> out;
    char oneLine[128];

    out.push_back( "                                Facility      Enabled State     Log Level" );
    out.push_back( "-------------------------------------------------------------------------" );

    for( auto const& fac : m_FacilityTable )
    {
        int index = fac.second;
        snprintf( oneLine, sizeof( oneLine ),
                  "%40s          %s               %d",
                  m_FacilityNames[index].c_str(),
                  m_FacilityEnabled[index] ? " on" : "off",
                  m_FacilityLogLevel[index] );
        out.push_back( oneLine );
    }

    out.push_back( "" );
    out.push_back( "" );
    out.push_back( "Log levels to choose from (larger values yield more output):" );

    constexpr auto numLevels = sizeof( levelNames ) / sizeof( levelNames[0] );
    for( size_t level = 0; level < numLevels; ++level )
    {
        snprintf( oneLine, sizeof( oneLine ), "%10s (%d)",
                  levelNames[level], level );
        out.push_back( oneLine );
    }

    out.push_back( "" );
    out.push_back( "Examples:" );
    out.push_back( "          loglevel 2             (sets log level to WARN)" );
    out.push_back( "          loglevel cli off       (disable logging for CLI)" );
    out.push_back( "          loglevel startup on    (enable logging for STARTUP)" );
    out.push_back( "          loglevel EVENTS on 3   (enable INFO logging for EVENTS)" );
    out.push_back( "          loglevel all off       (disable logging for all facilities)" );

    return out;
}

void DPrintImpl::LoadConfigFile()
{
    /* Load the config file but only once. */
    static bool loaded = false;
    if( loaded )
        return;
    //CThreadMutexLocker lock( &m_singletonSem );
    if( loaded )
        return;
    loaded = true;

    static const char envar[] = "BOSE_DPRINT_CONF";
    char const* conf = getenv( envar );
    if( conf )
    {
        /*
          If the first character of $BOSE_DPRINT_CONF is a dot or slash, then
          it's assumed to be the name of a file containing the configuration
          text.  Otherwise, it's assumed to be the actual configuration text.
          For example:

            $ env BOSE_DPRINT_CONF=./dprint.conf ./mytestprog

          is equivalent to

            $ env BOSE_DPRINT_CONF="$(cat ./dprint.conf)" ./mytestprog

          A more common usage would be something like:

            $ env BOSE_DPRINT_CONF="set stdout; all verbose" ./mytestprog

          http://hepdwiki.bose.com/twiki/bin/view/Projects/DebuggingWithDPrint
        */
        if( *conf == '/' or * conf == '.' )
            LoadConfigFile( conf );
        else
        {
            std::istringstream is( conf );
            LoadConfigFile( is, envar );
        }
        return;
    }
#if 0
    conf = "/media/sda1/" DPRINT_CONF_FILENAME;
    if( access( conf, F_OK ) == 0 )
    {
        SetLogOutputLocation( DPrintImpl::USB );
        LoadConfigFile( conf );
        return;
    }

    conf = "/etc/opt/Bose/" DPRINT_CONF_FILENAME;
    if( access( conf, F_OK ) == 0 )
    {
        LoadConfigFile( conf );
        return;
    }

    LoadConfigFile( DPRINT_CONF_FILE );
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Load default debug facilities from a config file
/// @param confFile - the path to the config file
/// @return none
////////////////////////////////////////////////////////////////////////////////
void DPrintImpl::LoadConfigFile( const char* confFileName )
{
    std::ifstream confFile;
    confFile.open( confFileName, std::ifstream::in );

    if( !confFile.is_open() )
    {
//        if( errno != ENOENT )
 //           std::clog << "open " << confFileName << ": " << ERR << std::endl;
        return;
    }
    LoadConfigFile( confFile, confFileName );
}

void DPrintImpl::LoadConfigFile( std::istream& inf,
                                 const char* confFileName )
{
    char c;
    int line_number = 1;
    Strings args;
    while( inf.get( c ) )
    {
        if( c == '\n' )
        {
            ProcessConfigCommand( confFileName, line_number, args );
            args.clear();
            ++line_number;
            continue;
        }
        if( c == ';' )
        {
            ProcessConfigCommand( confFileName, line_number, args );
            args.clear();
            continue;
        }
        if( c == '#' ) // Comment to end of line.
        {
            while( inf.get( c ) )
                if( c == '\n' )
                {
                    /* Put the newline back into the input stream to get
                       processed like any other newline. */
                    inf.unget();
                    break;
                }
            continue;
        }
        while( isspace( static_cast<unsigned char>( c ) ) and c != '\n' )
            if( not inf.get( c ) )
                goto breakboth;
        string arg;
        for( ;; )
        {
            if( c == ';' or c == '#' or
                isspace( static_cast<unsigned char>( c ) ) )
            {
                inf.unget();
                break;
            }
            arg += c;
            if( not inf.get( c ) )
                break;
        }
        if( not arg.empty() )
            args.emplace_back( std::move( arg ) );
    }
breakboth:
    /* If the configuration text didn't end with a newline, then there may
       have been some unprocessed arguments.  Do them now. */
    ProcessConfigCommand( confFileName, line_number, args );
}

int DPrintImpl::StringToLevel( const std::string& level )
{
    // The following code makes assumptions about the actual
    // values of the Level enum members.
    static_assert( DPrint::CRITICAL == 0, "oops" );
    static_assert( DPrint::INSANE == 7, "oops" );

    /*
      See if it's a number.
    */
    auto s = level.c_str();
    if( *s >= '0' and * s <= '7' and s[1] == '\0' )
        return DPrint::CRITICAL + ( *s - '0' );

    /*
      See if it's a symbolic name.
    */
    static const char * names[] =
    {
        "critical", // 0
        "error",    // 1
        "warning",  // 2
        "info",     // 3
        "debug",    // 4
        "test_automation", // 5
        "verbose",  // 6
        "insane",   // 7
    };
    int i = 0;
    for( auto name : names )
    {
        if( level == name )
            return i;
        ++i;
    }

    return -1;
}

void DPrintImpl::ProcessConfigCommand
( const char* file_name, int line_number, Strings& args )
{
    if( args.empty() )
        return;                 // ignore blank lines

    if( ProcessConfigSet( file_name, line_number, args ) )
        return;

    // if the process was specified
    if( args.size() == 3 )
    {
        auto& facility = args[0];
        auto& level = args[1];
        auto& process = args[2];

        // if we are the specified process
        if( process == GetEXE() )
            ProcessConfig( file_name, line_number, facility, level );
        return;
    }
    if( args.size() == 2 )
    {
        auto& facility = args[0];
        auto& level = args[1];
        ProcessConfig( file_name, line_number, facility, level );
        return;
    }
    std::clog << file_name << ':' << line_number
              << ": Ignoring command |" << args[0] << '|'
              << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Process a "set" command from the dprint config file.
///
/// "set" is a reserved word in config file (i.e., not a facility name), and
/// it used to implement arbitrary DPrint configuration options.
///
/// @param filename The name of the config file (for error messages).
///
/// @param line The line number in the config file where this command came
/// from (for error messages).
///
/// @param args The arguments of the command.  args[0]=="set".
///
/// @return true if this line was a "set" command, false if the line was
/// something else and should be processed further.
///
////////////////////////////////////////////////////////////////////////////////
bool DPrintImpl::ProcessConfigSet( const char* file, int line, Strings& args )
{
    if( args.empty() or args[0] != "set" )
        return false;

    if( args.size() == 2 and args[1] == "stdout" )
    {
        std::cout << m_appName << "Logging to stdout at "
                  << file << " line " << line << std::endl;
        SetLogOutputLocation( STDOUT );
    }
    else if( args.size() == 2 and args[1] == "syslog" )
    {
        std::cout << m_appName << "Logging to syslog at "
                  << file << " line " << line << std::endl;
        SetLogOutputLocation( SYSLOG );
    }
    else if( args.size() == 2 and args[1] == "datetime" )
    {
        s_logDateTime = true;
        std::cout << m_appName << "Use date-time format for timestamps at "
                  << file << " line " << line << std::endl;
    }
    else
    {
        std::clog << m_appName << "Unknown 'set' command at "
                  << file << " line " << line << std::endl;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Process a line from the dprint config file
///
/// @param filename The name of the config file (for error messages).
///
/// @param line The line number in the config file where this command came
/// from (for error messages).
///
/// @param facility The facility name.
///
/// @param levelName The level name.
///
////////////////////////////////////////////////////////////////////////////////
void DPrintImpl::ProcessConfig( const char *filename, int line,
                                const std::string& facility,
                                const std::string& levelName )
{
    if( facility == "global" )
    {
        fprintf( stderr, "%s:%d: \"global\" keyword is deprecated, ignored\n",
                 filename, line );
        return;
    }

    if( levelName == "off" )
    {
        // this function handles the special keyword "all" and *
        SetFacilityEnabled( facility, false );
        return;
    }

    auto i = StringToLevel( levelName );
    if( i == -1 )
    {
        fprintf( stderr, "%s:%d: invalid level '%s'\n",
                 filename, line, levelName.c_str() );
        return;
    }
    auto level = static_cast<DPrint::Level>( i );
    SetFacilityEnabled( facility, true );
    SetFacilityLogLevel( facility, level );
}

/* This overload exists (rather than just let facility be converted implicitly
   to std::string) because it reduces code size by a non-negligble amount. */
DPrint::DPrint( char const* facility )
    : m_index( -1 )
{
    m_index = DPrintImpl::Instance()->RegisterDPrintClient( facility );
}

DPrint::DPrint( const string& facility )
    : m_index( -1 )
{
    m_index = DPrintImpl::Instance()->RegisterDPrintClient( facility );
}

DPrint::~DPrint()
{
    DPrintImpl::Instance()->UnregisterDPrintClient( m_index );

    /* If *this is a static object, it's possible that (due to static
       initialization & destruction ordering) some other static object may try
       to use *this during that other object's destruction.  We clear m_index
       to the special index 0 to handle that possibility more gracefully.  See
       also SHELBY-35386. */
    m_index = 0;
}

void DPrint::Initialize( std::string const& appName )
{
    GetSysLog().Initialize( appName );
    auto impl = DPrintImpl::Instance();

    {
        std::ostringstream ss;
        ss << appName << '[' << getpid() << ']';
        impl->m_appName = ss.str();
    }

    impl->LoadConfigFile();
}

DPrint::DPrint( const DPrint& that )
    : m_index( that.m_index )
{
    DPrintImpl::Instance()->RegisterDPrintClient( m_index );
}

DPrint& DPrint::operator=( const DPrint& that )
{
    m_index = that.m_index;
    DPrintImpl::Instance()->RegisterDPrintClient( m_index );
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to set the current log level (called by CLI command)
/// @param level the new log level
/// @return true if the log level was set, false otherwise
////////////////////////////////////////////////////////////////////////////////
bool DPrint::SetGlobalLogLevel( DPrint::Level level )
{
    return DPrintImpl::Instance()->SetGlobalLogLevel( level );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to set the logging level of the given
/// facility (called by CLI command)
/// @param facility the facility to have its level changed
/// @param DPrint::Level level - what level to set (change) it to
/// @return true if the facility enabled flag was changed, false otherwise
////////////////////////////////////////////////////////////////////////////////
bool DPrint::SetLogLevel( const string& facility, DPrint::Level level )
{
    return DPrintImpl::Instance()->SetFacilityLogLevel( facility, level );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to set the logging enabled flag of the given
/// facility (called by CLI command)
/// @param facility the facility to be enabled/disabled
/// @param enabled how to set the facility (true=enabled, false=disabled)
/// @return true if the facility enabled flag was changed, false otherwise
////////////////////////////////////////////////////////////////////////////////
bool DPrint::SetFacilityEnabled( const string& facility, bool enabled )
{
    return DPrintImpl::Instance()->SetFacilityEnabled( facility, enabled );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to set the logging enabled flag of all
/// facilities
/// @param enabled how to set the facility (true=enabled, false=disabled)
////////////////////////////////////////////////////////////////////////////////
void DPrint::SetEnabled( bool bEnabled )
{
    DPrintImpl::Instance()->SetEnabled( bEnabled );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to set the logging to stdout or syslog
/// @param enable true=stdout, false=syslog
////////////////////////////////////////////////////////////////////////////////
void DPrint::SetPrintToStdout( bool enable )
{
    DPrintImpl::Instance()->
    SetLogOutputLocation( enable ? DPrintImpl::STDOUT : DPrintImpl::SYSLOG );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to constructs a list of strings which together
/// provide the current status of the logger (i.e. current log level, list of
/// registered facilities along with their current enabled state).  Also provides
/// a basic level of help by displaying the log levels to choose from and a couple
/// of small examples.  This method is called by the loglevel CLI command.
/// @param None
/// @return the list of strings containing all the info
////////////////////////////////////////////////////////////////////////////////
list<string> DPrint::GetCurrentLoggingStatus() const
{
    return DPrintImpl::Instance()->GetCurrentLoggingStatus( );
}

void DPrint::Log( DPrint::Level level, const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->Log( level, m_index, format, args );
    va_end( args );
}

bool DPrint::IsLog( DPrint::Level level ) const
{
    return DPrintImpl::Instance()->IsLog( level, m_index );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is CRITICAL
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogCritical( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( CRITICAL, m_index, format, args );
    va_end( args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is ERROR is lower
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogError( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( ERROR, m_index, format, args );
    va_end( args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is WARNING is lower
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogWarning( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( WARNING, m_index, format, args );
    va_end( args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is INFO is lower
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogInfo( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( INFO, m_index, format, args );
    va_end( args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is DEBUG is lower
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogDebug( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( DEBUG, m_index, format, args );
    va_end( args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is VERBOSE is lower
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogVerbose( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( VERBOSE, m_index, format, args );
    va_end( args );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Interface method used to log a message if the current logging level
/// is INSANE is lower (log everything)
/// @param format the printf formatted string to be logged
////////////////////////////////////////////////////////////////////////////////
void DPrint::LogInsane( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogIfRequired( INSANE, m_index, format, args );
    va_end( args );
}

void DPrint::Prefix( std::ostringstream& ss, Level level,
                     char const* func ) const
{
    DPrintImpl::Instance()->Prefix( ss, level, func, m_index );
}

void DPrint::Put( std::ostringstream& ss ) const
{
    DPrintImpl::Instance()->Put( ss );
}

////////////////////////////////////////////////////////////////////////////////

/*
  Test Automation Logging.

  This is all legacy deprecated vestigial stuff.  Don't use it.
 */

#include <atomic>

static std::atomic<DPrint::TestAutomationLogHandler*> s_testAutoLogHandler;

void DPrintImpl::LogForTestAutomation( int facilityIndex,
                                       const char* format, va_list args )
{
    DPrint::TestAutomationLogHandler* handler = s_testAutoLogHandler;
    if( not handler )
        return;

    // STiG scripts allegedly depend on the format of this prefix,
    // so tread lightly.
    char buffer[2048];
    int len = snprintf( buffer, sizeof( buffer ), "[%s  %s:TESTAUTO] ",
                        Timestamp().generate(),
                        m_FacilityNames[facilityIndex].c_str() );
    if( len == -1 )
        return;
    size_t ulen = len;
    if( ulen >= sizeof( buffer ) )
        return;
    len = vsnprintf( buffer + ulen, sizeof( buffer ) - ulen, format, args );
    if( len == -1 )
        // format string was malformed
        len = snprintf( buffer + ulen, sizeof( buffer ) - ulen, "*OOPS*" );

    ulen += len;
    if( ulen >= sizeof( buffer ) )
        ulen = sizeof( buffer ) - 1;

    /*
      Make sure there's a newline terminator.
    */
    if( ulen == 0 or buffer[ulen - 1] != '\n' )
    {
        if( ulen == sizeof( buffer ) - 1 )
            --ulen;
        buffer[ulen] = '\n';
        buffer[++ulen] = '\0';
    }

    handler( buffer, ulen );
}

void DPrint::LogTestAutomation( const char* format, ... ) const
{
    va_list args;
    va_start( args, format );
    DPrintImpl::Instance()->LogForTestAutomation( m_index, format, args );
    va_end( args );
}

void DPrint::SetTestAutomationLogHandler( TestAutomationLogHandler* handler )
{
    s_testAutoLogHandler = handler;
}

////////////////////////////////////////////////////////////////////////////////

static struct
{
    char const* name;
    DPrint::Level level;
} const names[] =
{
    { "0",        DPrint::CRITICAL },
    { "critical", DPrint::CRITICAL },
    { "1",        DPrint::ERROR },
    { "error",    DPrint::ERROR },
    { "2",        DPrint::WARNING },
    { "warning",  DPrint::WARNING },
    { "3",        DPrint::INFO },
    { "info",     DPrint::INFO },
    { "4",        DPrint::DEBUG },
    { "debug",    DPrint::DEBUG },
    { "6",        DPrint::VERBOSE },
    { "verbose",  DPrint::VERBOSE },
    { "7",        DPrint::INSANE },
    { "insane",   DPrint::INSANE },

    /* deprecated/historical */
    { "5",        DPrint::UNUSED },
    { "test",     DPrint::UNUSED },
    { "test_automation", DPrint::UNUSED },
};

DPrint::Level DPrint::NameToLevel( std::string const& name )
{
    for( auto & n : names )
        if( strcasecmp( name.c_str(), n.name ) == 0 )
            return n.level;
    throw std::out_of_range( name );
}

std::ostream& operator<<( std::ostream& os, DPrint::Limit const& s )
{
    if( s.len_ <= s.lim_ + 3 )     // +3 for "..."
        return os.write( s.p_, s.len_ );
    return os.write( s.p_, s.lim_ ) << "...";
}
