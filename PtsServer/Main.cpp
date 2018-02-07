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
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

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

std::string to_string( sockaddr_storage const& addr, socklen_t len )
{
    switch( addr.ss_family )
    {
    case AF_INET:
    {
        auto& a = reinterpret_cast<sockaddr_in const&>( addr );
        if( len < sizeof( a ) )
        {
            LOG( "Invalid sockaddr length: "  << len );
            return {};
        }
        char buf[INET_ADDRSTRLEN];
        if( inet_ntop( AF_INET, &a.sin_addr, buf, sizeof( buf ) ) == nullptr )
        {
            LOG( "inet_ntop: " << err() );
            return {};
        }
        return buf;
    }
    case AF_INET6:
    {
        auto& a = reinterpret_cast<sockaddr_in6 const&>( addr );
        if( len < sizeof( a ) )
        {
            LOG( "Invalid sockaddr length: "  << len );
            return {};
        }
        char buf[INET6_ADDRSTRLEN];
        if( inet_ntop( AF_INET6, &a.sin6_addr, buf, sizeof( buf ) ) == nullptr )
        {
            LOG( "inet_ntop: " << err() );
            return {};
        }
        return buf;
    }
    default:
    {
        LOG( "Invalid sockaddr family: " << addr.ss_family );
        return {};
    }
    }
}

void handle_connection( int sockfd, std::string const& clientaddrstr )
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

        auto env = "REMOTE_ADDRESS=" + clientaddrstr;
        if( putenv( const_cast< char* >( env.c_str() ) ) != 0 )
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

        LOG( "accept " << clientaddrstr );

        handle_connection( connfd, clientaddrstr );
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
