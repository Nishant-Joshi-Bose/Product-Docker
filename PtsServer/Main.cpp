/*
  Trivial web server to serve PTS information and static web content.

  If you're thinking of extending this web server to do something more than it
  currently does, you should instead consider using a real web server.

  Usage: PtsServer /path/to/pts-handler

  The pts-handler program is invoked for each HTTP connection.
  HTTP request headers and payload are to be read from stdin.
  HTTP response headers and payload are to be written to stdout.
*/

#include <iostream>
#include <stdexcept>
#include <memory>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <ifaddrs.h>

namespace
{

char const* err()
{
    return strerror( errno );
}

char const* program_name;

#define LOG( EXPR )                                                     \
    std::clog << program_name << ": " << EXPR << std::endl

#define DIE( EXPR )                                                     \
    do {                                                                \
        LOG( EXPR );                                                    \
        _exit( EXIT_FAILURE );                                          \
    } while( 0 )

constexpr unsigned short listen_port = 80;

/* Handling each HTTP request must finish within this amount of time. */
constexpr unsigned int max_connection_seconds = 60;

char const* handler_program;

int open_listenfd()
{
    int listenfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( listenfd == -1 )
        DIE( "socket: " << err() );

    int v = 1;
    if( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR,
                    ( const void * )&v , sizeof( v ) ) == -1 )
        DIE( "SO_REUSEADDR: " << err() );

    sockaddr_in sa;
    memset( &sa, 0, sizeof( sa ) );
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl( INADDR_ANY );
    sa.sin_port = htons( listen_port );
    if( bind( listenfd, ( sockaddr * )&sa, sizeof( sa ) ) == -1 )
        DIE( "bind: " << err() );

    if( listen( listenfd, 64 ) == -1 )
        DIE( "listen: " << err() );

    return listenfd;
}

std::string to_string( sockaddr_in const& addr )
{
    char buf[INET_ADDRSTRLEN];
    if( inet_ntop( AF_INET, &addr.sin_addr, buf, sizeof( buf ) ) == nullptr )
    {
        LOG( "inet_ntop: " << err() );
        return {};
    }
    return buf;
}

std::string to_string( sockaddr_in6 const& addr )
{
    char buf[INET6_ADDRSTRLEN];
    if( inet_ntop( AF_INET6, &addr.sin6_addr, buf, sizeof( buf ) ) == nullptr )
    {
        LOG( "inet_ntop: " << err() );
        return {};
    }
    return buf;
}

std::string to_string( sockaddr_storage const& addr, socklen_t len )
{
    switch( addr.ss_family )
    {
    case AF_INET:
    {
        auto& a = reinterpret_cast<sockaddr_in const&>( addr );
        if( len < sizeof( a ) )
        {
            LOG( "Invalid sockaddr length: " << len );
            return {};
        }
        return to_string( a );
    }
    case AF_INET6:
    {
        auto& a = reinterpret_cast<sockaddr_in6 const&>( addr );
        if( len < sizeof( a ) )
        {
            LOG( "Invalid sockaddr length: " << len );
            return {};
        }
        return to_string( a );
    }
    default:
    {
        LOG( "Invalid sockaddr family: " << addr.ss_family );
        return {};
    }
    }
}

std::string get_iface_name( sockaddr_in const& addr )
{
    ifaddrs *ifa = nullptr;
    if( getifaddrs( &ifa ) == -1 )
    {
        LOG( "getifaddrs: " << err() );
        return {};
    }
    std::unique_ptr<ifaddrs, void( * )( ifaddrs* )>
    finally{ ifa, freeifaddrs };

    for( ; ifa; ifa = ifa->ifa_next )
    {
        if( !ifa->ifa_addr )
            continue;
        if( !ifa->ifa_name )
            continue;
        if( ifa->ifa_addr->sa_family != AF_INET )
            continue;
        auto& a = reinterpret_cast<sockaddr_in&>( *ifa->ifa_addr );
        if( a.sin_addr.s_addr == addr.sin_addr.s_addr )
            return ifa->ifa_name;
    }

    LOG( "No interface found for IPv4 address " << to_string( addr ) );
    return {};
}

std::string get_iface_name( sockaddr_in6 const& addr )
{
    ifaddrs *ifa = nullptr;
    if( getifaddrs( &ifa ) == -1 )
    {
        LOG( "getifaddrs: " << err() );
        return {};
    }
    std::unique_ptr<ifaddrs, void( * )( ifaddrs* )>
    finally{ ifa, freeifaddrs };

    for( ; ifa; ifa = ifa->ifa_next )
    {
        if( !ifa->ifa_addr )
            continue;
        if( !ifa->ifa_name )
            continue;
        if( ifa->ifa_addr->sa_family != AF_INET6 )
            continue;
        auto& a = reinterpret_cast<sockaddr_in6&>( *ifa->ifa_addr );
        if( memcmp( a.sin6_addr.s6_addr,
                    addr.sin6_addr.s6_addr,
                    sizeof( a.sin6_addr.s6_addr ) ) == 0 )
            return ifa->ifa_name;
    }

    LOG( "No interface found for IPv6 address " << to_string( addr ) );
    return {};
}

std::string get_iface_name( sockaddr_storage const& addr, socklen_t len )
{
    switch( addr.ss_family )
    {
    case AF_INET:
    {
        auto& a = reinterpret_cast<sockaddr_in const&>( addr );
        if( len < sizeof( a ) )
        {
            LOG( "Invalid sockaddr length: " << len );
            return {};
        }
        return get_iface_name( a );
    }
    case AF_INET6:
    {
        auto& a = reinterpret_cast<sockaddr_in6 const&>( addr );
        if( len < sizeof( a ) )
        {
            LOG( "Invalid sockaddr length: " << len );
            return {};
        }
        return get_iface_name( a );
    }
    default:
    {
        LOG( "Invalid sockaddr family: " << addr.ss_family );
        return {};
    }
    }
}

std::string get_iface_name( int sockfd )
{
    sockaddr_storage addr;
    socklen_t len = sizeof( addr );
    if( getsockname( sockfd, ( sockaddr* )&addr, &len ) == -1 )
    {
        LOG( "getsockname: " << err() );
        return {};
    }
    //LOG( "getsockname '" << to_string( addr, len ) << '\'' );
    return get_iface_name( addr, len );
}

void handle_connection( int sockfd, std::string const& clientaddrstr,
                        std::string const& iface )
{
    auto kid = fork();
    if( kid == -1 )
    {
        LOG( "fork: " << err() );
        return;
    }

    if( kid == 0 )
    {
        // In the child.
        if( dup2( sockfd, STDIN_FILENO ) == -1 )
            LOG( "dup2: " << err() );
        if( dup2( sockfd, STDOUT_FILENO ) == -1 )
            LOG( "dup2: " << err() );
        close( sockfd );

        alarm( max_connection_seconds );

        signal( SIGCHLD, SIG_DFL );

        /* putenv keeps a pointer to these temporary std::string objects.
           This code assumes we execl and the temporary objects never actually
           get destroyed. */

        auto env1 = "REMOTE_ADDRESS=" + clientaddrstr;
        if( putenv( const_cast< char* >( env1.c_str() ) ) != 0 )
            LOG( "putenv: " << err() );

        auto env2 = "IFACE=" + iface;
        if( putenv( const_cast< char* >( env2.c_str() ) ) != 0 )
            LOG( "putenv: " << err() );

        execl( handler_program, handler_program, nullptr );
        DIE( "execl " << handler_program << ": " << err() );
    }
    close( sockfd );
}

void listener( int listenfd )
{
    for( ;; )
    {
        sockaddr_storage clientaddr;
        socklen_t clientlen = sizeof( clientaddr );
        int connfd = accept( listenfd, ( sockaddr * )&clientaddr, &clientlen );
        if( connfd == -1 )
        {
            LOG( "accept: " << err() );
            break;
        }
        auto clientaddrstr = to_string( clientaddr, clientlen );
        auto iface = get_iface_name( connfd );

        LOG( "accept " << clientaddrstr << " on " << iface );

        handle_connection( connfd, clientaddrstr, iface );
        close( connfd );
    }
}

} // namespace

int main( int argc, char** argv )
try
{
    program_name = strrchr( argv[0], '/' );
    if( program_name )
        ++program_name;
    else
        program_name = argv[0];

    while( --argc > 0 )
    {
        auto arg = *++argv;

        if( handler_program == nullptr )
        {
            handler_program = arg;
            continue;
        }

        DIE( "Unexpected argument '" << arg << '\'' );
    }

    if( !handler_program )
        DIE( "No handler program specified" );

    signal( SIGCHLD, SIG_IGN );

    int listenfd = open_listenfd();

    /* We could potentially launch multiple threads here each to invoke
       listener() to allow multiple parallel client connections.  That would
       tend to make the web server more responsive if responsiveness were
       found to be an issue. */

    LOG( "Ready" );
    listener( listenfd );
    close( listenfd );
    LOG( "Goodbye" );
}
catch( std::exception const& exc )
{
    DIE( "Caught top level exception: " << exc.what() );
}
