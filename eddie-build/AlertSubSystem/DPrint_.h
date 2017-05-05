////////////////////////////////////////////////////////////////////////////////
/// @file            DPrint.h
/// @brief           Logging.
///
/// Copyright 2011 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <list>
#include <vector>
#include <cstring>
#include <cerrno>
#include <sstream>

class DPrint
{
public:
    enum Level
    {
        CRITICAL,        // Catastrophic.
        ERROR,           // Bad, probably unrecoverable.
        WARNING,         // Bad but recoverable.
        INFO,            // Ordinary.
        DEBUG,           // Normal debugging.
        UNUSED,          // Not a real log level. Don't use.
        VERBOSE,         // Verbose debugging.
        INSANE           // Very verbose debugging.
    };

    /*!
     * Construct a DPrint logger object by registering the given facility name
     * with the DPrint infrastructure.  The facility name is intended to
     * identify the component that is doing the logging.
     *
     * @param facility The name.
     */
    explicit DPrint( char const* facility );
    explicit DPrint( const std::string& facility );

    static bool SetGlobalLogLevel( Level level );
    bool SetLogLevel( const std::string& facility, Level level );
    bool SetFacilityEnabled( const std::string& facility, bool enabled );
    void SetEnabled( bool bEnabled );
    std::list<std::string> GetCurrentLoggingStatus( ) const;

    static void SetPrintToStdout( bool enable = true );

    ~DPrint();

    /*!
     * Initialize the DPrint component.
     *
     * Sets the process/application name (to be included in log messages) and
     * loads any dprint.conf settings.
     *
     * See http://hepdwiki.bose.com/bin/view/Projects/DebuggingWithDPrint
     *
     * This function should be called before the first message is logged.
     * Calling afterwards changes the identity, which is usually undesireable,
     * so you probably shouldn't call twice.  If not called, a default identity
     * is used and no dprint.conf settings are loaded.
     */
    static void Initialize( std::string const& appName );

    /*!
     * Check if the given log level is enabled.
     *
     * Prefer BOSE_IS_LOG because it takes into account the compile-time log
     * level.
     *
     * @param level The log level
     *
     * @return true if logging at this level is enabled, false if invoking the
     * corresponding Log... function will have no effect.
     */
    bool IsLog( Level level ) const;

    void LogCritical( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );
    void LogError( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );
    void LogWarning( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );
    void LogInfo( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );
    void LogDebug( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );
    void LogVerbose( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );
    void LogInsane( const char* format, ... ) const __attribute__( ( format( printf, 2, 3 ) ) );

    /*!
     * Log a message unconditionally (regardless of log level).
     *
     * You should have checked IsLog before calling this function.
     * This is a helper function for the BOSE_LOG macro.
     * You probably don't want to be calling this function directly.
     *
     * @param level The log level.
     *
     * @param format The printf-style format string.  There should be no
     * trailing newline, but if present it will be discarded.
     */
    void Log( Level level, const char* format, ... )
    const __attribute__( ( format( printf, 3, 4 ) ) );

    /*!
     * Send the given log message to the TestAutomationLogHandler, if any.
     *
     * @deprecated
     */
    void LogTestAutomation( const char* format, ... )
    const __attribute__( ( format( printf, 2, 3 ) ) );

    /*!
     * Function to call for LogTestAutomation messages.
     *
     * @deprecated
     *
     * @param message The log message with a timestamp prefix and a trailing
     * newline '\n' and null terminator '\0'.
     *
     * @param len The length of the message excluding the null terminator
     * (i.e., strlen).
     */
    typedef void TestAutomationLogHandler( char const* message, size_t len );

    /*!
     * Set the Test Automation log handler.
     *
     * @deprecated
     *
     * @param h The function to call for LogTestAutomation, or nullptr.
     */
    static void SetTestAutomationLogHandler( TestAutomationLogHandler* h );

    DPrint( const DPrint& );
    DPrint& operator=( const DPrint& );

    /*!
     * Convert a log level string name to the enum value.
     *
     * @param name The log level string name or number, e.g., "critical"
     * "CRITICAL" or "0".
     *
     * @return The log level associated with the name.  Or throws
     * std::out_of_range if the name is not valid.
     */
    static Level NameToLevel( std::string const& name );

    /*!
     * Helper function for the BOSE_LOG macro.
     *
     * Writes a log message prefix to the given stream.
     *
     * @param oss The stream to write to.
     *
     * @param level The log level.
     *
     * @param func The name of the current function (__func__).
     */
    void Prefix( std::ostringstream& oss, Level level, char const* func ) const;

    /*!
     * Helper function for the BOSE_LOG macro.
     *
     * Logs a message from a string stream.
     *
     * @param oss The stream containing the log message.
     */
    void Put( std::ostringstream& oss ) const;

    /*
     * Helper for logging a string that might be very long.
     * Example:
     *
     *   BOSE_LOG( INFO, "Message: " << DPrint::Limit( xml ) );
     *
     * If the string is too long, show only the beginning and append "...".
     *
     * Avoids copying the string, but assumes the string object continues to
     * exist for a while.  For example:
     *
     *   BOSE_LOG( INFO, "ok: " << DPrint::Limit( a + b ) );
     *   DPrint::Limit s( a + b );
     *   BOSE_LOG( INFO, "not ok: " << s );
     *
     * a+b creates a temporary which is destroyed before the following
     * BOSE_LOG statement.
     */
    struct Limit
    {
        static constexpr size_t defaultLimit = 100;
        char const* p_;
        size_t len_;
        size_t lim_;
        Limit( std::string const& s, size_t lim = defaultLimit )
            : p_( s.data() ),
              len_( s.size() ),
              lim_( lim )
        {
        }
        Limit( char const* s, size_t lim = defaultLimit )
            : p_( s ),
              len_( strlen( s ) ),
              lim_( lim )
        {
        }
    };

private:

    /*! A small integer to uniquely identify the facility. */
    int m_index;
};

std::ostream& operator<<( std::ostream& , DPrint::Limit const& );

/*
 * BOSE_LOG_LEVEL is the compile-time log level.  Any log messages that are
 * above the given level are compiled out of the executables and cannot be
 * enabled at run-time.  This level is normally determined by the makefiles,
 * e.g., make LOG_LEVEL=INSANE
 */
#if !defined( BOSE_LOG_LEVEL )
#define BOSE_LOG_LEVEL DPrint::INSANE
#endif

/*!
 * Check the compile-time and run-time log levels.
 */
#define BOSE_IS_LOG( LOGGER, LEVEL ) \
    ( ( LEVEL ) <= BOSE_LOG_LEVEL and (LOGGER).IsLog( LEVEL ) )

/*!
 * Log at the given level if that level is enabled.
 *
 * @param LOGGER The DPrint object.
 *
 * @param LEVEL The DPrint::Level log level.
 *
 * @param ... A printf-style format string and any arguments.  There should be
 * no trailing newline, but if present it will be discarded.
 */
#define BOSE_LOG_IF( LOGGER, LEVEL, ... )                               \
    do {                                                                \
        if( BOSE_IS_LOG( LOGGER, LEVEL ) )                              \
            (LOGGER).Log( LEVEL, __VA_ARGS__ );                         \
    } while ( 0 )

/*!
 * More efficient logging.
 *
 * Instead of:
 *   s_logger.LogDebug( "x=%s", SomeExpensiveOperation().c_str() );
 * we should prefer:
 *   BOSE_DEBUG( s_logger, "x=%s", SomeExpensiveOperation().c_str() );
 *
 * The latter avoids executing SomeExpensiveOperation() if the debug log level
 * is disabled.
 *
 * It makes sense to always use these macros for DEBUG, VERBOSE and
 * INSANE because those log levels are disabled by default.
 *
 * The script Scripts/convert-logs updates a *.cpp source file changing all
 * occurrences of LogDebug, Verbose and Insane to BOSE_DEBUG, VERBOSE and
 * INSANE.  With no arguments, it updates any C++ source files that are
 * already modified in your workspace.
 */
#define BOSE_CRITICAL( LOGGER, ... ) BOSE_LOG_IF( LOGGER, DPrint::CRITICAL, __VA_ARGS__ )
#define BOSE_ERROR( LOGGER, ... )    BOSE_LOG_IF( LOGGER, DPrint::ERROR, __VA_ARGS__ )
#define BOSE_WARNING( LOGGER, ... )  BOSE_LOG_IF( LOGGER, DPrint::WARNING, __VA_ARGS__ )
#define BOSE_INFO( LOGGER, ... )     BOSE_LOG_IF( LOGGER, DPrint::INFO, __VA_ARGS__ )
#define BOSE_DEBUG( LOGGER, ... )    BOSE_LOG_IF( LOGGER, DPrint::DEBUG, __VA_ARGS__ )
#define BOSE_VERBOSE( LOGGER, ... )  BOSE_LOG_IF( LOGGER, DPrint::VERBOSE, __VA_ARGS__ )
#define BOSE_INSANE( LOGGER, ... )   BOSE_LOG_IF( LOGGER, DPrint::INSANE, __VA_ARGS__ )

/*
 * Log a warning about the current errno (a la perror(3)).
 *
 * @param LOGGER The DPrint object to log with.
 *
 * @param LABEL A string describing what operation failed.
 *
 * Example:
 *   if( sendto(...) == -1 )
 *       BOSE_PERROR( g_logger, "sendto" );
 */
#define BOSE_PERROR( LOGGER, LABEL )                                    \
    BOSE_WARNING( LOGGER, "%s: %s at %s line %d",                       \
                  LABEL, ::strerror( errno ), __FILE__, __LINE__ )

/*!
 * Type-safe logging.
 *
 * A simplifying wrapper on the BOSE_LOGX macro.  Assumes you have declared a
 * DPrint object named "s_logger" in scope.  Implicitly prefixes DPrint:: to
 * the LEVEL argument.
 *
 * @param LEVEL The DPrint::Level enum name without the DPrint:: prefix.
 *
 * @param EXPR An expression suitable for writing to a std::ostream.
 *
 * Example:
 *   static DPrint s_logger( "MyThing" );
 *   BOSE_LOG( INFO, "x=" << x << ", y=" << y );
 */
#define BOSE_LOG( LEVEL, EXPR ) BOSE_LOGX( s_logger, DPrint::LEVEL, EXPR )

/*!
 * Type-safe logging.
 *
 * Prefer to use the simpler BOSE_LOG instead of BOSE_LOGX.
 *
 * @param LOGGER The DPrint object.
 *
 * @param LEVEL The DPrint::Level log level.
 *
 * @param EXPR An expression suitable for writing to a std::ostream.
 *
 * Example:
 *   static DPrint s_logger( "MyThing" );
 *   BOSE_LOGX( s_logger, DPrint::INFO, "x=" << x << ", y=" << y );
 */
#define BOSE_LOGX( LOGGER, LEVEL, EXPR )                                \
    do {                                                                \
        if( BOSE_IS_LOG( LOGGER, LEVEL ) )                              \
        {                                                               \
            std::ostringstream DPrint_ss;                               \
            LOGGER.Prefix( DPrint_ss, LEVEL, __func__ );                \
            DPrint_ss << EXPR;                                          \
            LOGGER.Put( DPrint_ss );                                    \
        }                                                               \
    } while ( 0 )

/*!
 * Log a critical error message (both to stderr and the current log
 * destination) and then exit.
 *
 * Equivalent to:
 *
 *  BOSE_LOG( CRITICAL, EXPR );
 *  std::clog << EXPR << std::endl;
 *  exit( EXIT_FAILURE );
 *
 */
#define BOSE_DIE( EXPR )                                                \
    do {                                                                \
        std::ostringstream DPrint_ss;                                   \
        s_logger.Prefix( DPrint_ss, DPrint::CRITICAL, __func__ );       \
        DPrint_ss << EXPR;                                              \
        s_logger.Put( DPrint_ss );                                      \
        std::clog << DPrint_ss.str() << std::endl;                      \
        ::exit( EXIT_FAILURE );                                         \
    } while( 0 )
