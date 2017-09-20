/*
  Trivial web server to serve PTS information.

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

void handle_connection( int sockfd )
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

        execl( handler_program, handler_program, nullptr );
        DIE( "execl " << handler_program << ": " << err() );
    }
    close( sockfd );

    int status = 0;
    if( waitpid( kid, &status, 0 ) == -1 )
        LOG( "waitpid: " << err() );
    else
    {
        if( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
            LOG( "Child exited " << WEXITSTATUS( status ) );
        if( WIFSIGNALED( status ) )
            LOG( "Child signaled " << WTERMSIG( status ) );
    }
}

void listener( int listenfd )
{
    for( ;; )
    {
        sockaddr_in clientaddr;
        socklen_t clientlen = sizeof( clientaddr );
        int connfd = accept( listenfd, ( sockaddr * )&clientaddr, &clientlen );
        if( connfd == -1 )
        {
            LOG( "accept: " << err() );
            break;
        }

        LOG( "accept " << inet_ntoa( clientaddr.sin_addr )
             << ':' << ntohs( clientaddr.sin_port ) );

        handle_connection( connfd );
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

    signal( SIGPIPE, SIG_IGN );

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
