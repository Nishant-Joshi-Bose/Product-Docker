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
#include <fstream>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <sys/sendfile.h>

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

std::string get_iface_name( int sockfd, std::string& local_address )
{
    sockaddr_storage addr;
    socklen_t len = sizeof( addr );
    if( getsockname( sockfd, ( sockaddr* )&addr, &len ) == -1 )
    {
        LOG( "getsockname: " << err() );
        local_address.erase();
        return {};
    }
    local_address = to_string( addr, len );
    return get_iface_name( addr, len );
}

// Map of URL path to local file system.
struct static_item
{
    std::string file_name;
    std::string mime_type;
};
std::unordered_map<std::string, static_item> static_items;

using strings = std::vector<std::string>;

strings split( std::string const& line )
{
    strings fields;
    size_t i = 0;
    for( ;; )
    {
        i = line.find_first_not_of( " \t", i );
        if( i == std::string::npos )
            break;
        size_t j = line.find_first_of( " \t", i );
        if( j == std::string::npos )
        {
            fields.emplace_back( line.substr( i ) );
            break;
        }
        fields.emplace_back( line.substr( i, j - i ) );
        i = j;
    }
    return fields;
}

void read_static_content_map( char const* map_file_name )
{
    /*
      Each line of the map file is:

      url_path file_name mime_type
    */
    std::ifstream ifs{ map_file_name };
    if( !ifs )
    {
        LOG( "open " << map_file_name << ": " << err() );
        return;
    }
    std::string line;
    int line_number = 0;
    while( getline( ifs, line ) )
    {
        ++line_number;
        strings fields = split( line );
        if( fields.empty() )
            continue;

        if( fields.size() != 3 )
        {
            LOG( "Malformed map at " << map_file_name << " line " << line_number );
            continue;
        }
        auto const& url_path = fields[0];
        auto const& file_name = fields[1];
        auto const& mime_type = fields[2];

        auto r = static_items.emplace( url_path, static_item{ file_name, mime_type } );
        if( !r.second )
            LOG( "Duplicate entry for " << url_path
                 << " at " << map_file_name << " line " << line_number );
    }
}

void write_all( int fd, char const* data, size_t len )
{
    while( len != 0 )
    {
        ssize_t r = write( fd, data, len );
        if( r == -1 )
        {
            LOG( "write: " << err() );
            break;
        }
        if( r == 0 || size_t( r ) > len )
        {
            LOG( "strange write: " << r << ' ' << len );
            break;
        }
        data += r;
        len -= r;
    }
}

/* If url_path refers to static content, serve it and return true.
   Otherwise return false. */
bool send_static_content( std::string url_path )
{
    auto it = static_items.find( url_path );
    if( it == static_items.end() )
        return false;
    auto const& file_name = it->second.file_name;
    auto const& mime_type = it->second.mime_type;
    LOG( "Serve static " << url_path << " as " << file_name << ' ' << mime_type );

    // Send the HTTP response header
    constexpr char part0[] = "HTTP/1.1 200 OK\r\nContent-Type: ";
    constexpr char part2[] = "\r\nCache-Control: no-cache\r\n\r\n";
    std::string rsp;
    rsp.reserve( sizeof( part0 ) - 1 +
                 mime_type.size() +
                 sizeof( part2 ) - 1 );
    rsp.append( part0, sizeof( part0 ) - 1 );
    rsp.append( mime_type );
    rsp.append( part2, sizeof( part2 ) - 1 );
    write_all( STDOUT_FILENO, rsp.data(), rsp.size() );

    // Send the whole file as the HTTP payload.
    int in_fd = open( file_name.c_str(), O_RDONLY );
    if( in_fd == -1 )
    {
        LOG( "open " << file_name << ": " << err() );
        return true;
    }
    ssize_t r = sendfile( STDOUT_FILENO, in_fd, nullptr, SIZE_MAX );
    close( in_fd );
    if( r == -1 )
        LOG( "sendfile: " << err() );
    return true;
}

void handle_connection( int sockfd, std::string const& remote_address,
                        std::string const& local_address,
                        std::string const& iface )
{
    auto kid = fork();
    if( kid == -1 )
    {
        LOG( "fork: " << err() );
        return;
    }

    if( kid != 0 )
    {
        // In the parent process.
        close( sockfd );
        return;
    }

    // In the child process.
    alarm( max_connection_seconds );
    signal( SIGCHLD, SIG_DFL );

    if( dup2( sockfd, STDIN_FILENO ) == -1 )
        LOG( "dup2: " << err() );
    if( dup2( sockfd, STDOUT_FILENO ) == -1 )
        LOG( "dup2: " << err() );
    close( sockfd );

    /* Read the HTTP headers.

       The first line is like: "GET /foo/bar HTTP/1.1\r\n" where "GET" is the
       HTTP method and "/foo/bar" is the path component of the URL.

       Next are zero or more non-blank lines containing the headers like
       "Host: www.example.com\r\n".

       Finally, there is a blank line "\r\n".

       There may be more data after the blank line.  We do not remove that
       data from the input stream but instead leave it for use by the
       pts-handler script (e.g., for POST methods).
    */
    std::string h;
    bool at_line_start = true;
    for( ;; )
    {
        char c;
        ssize_t r = read( STDIN_FILENO, &c, 1 );
        if( r == -1 )
            DIE( "handle_connection: read: " << err() );
        if( r == 0 )
            DIE( "handle_connection: EOF" );
        if( c == '\n' )
        {
            // Discard carriage returns to simplify parsing.
            if( !h.empty() && h.back() == '\r' )
                h.pop_back();
            if( at_line_start )
                break;
            at_line_start = true;
        }
        else if( c != '\r' )
            at_line_start = false;
        h.push_back( c );
    }

    /* Modifying this std::string object is undefined behavior, but in
       practice it's fine here. */
    auto p = const_cast<char*>( h.c_str() );
    auto field = [&]()
    {
        char* start = p;
        while( *p )
        {
            if( *p == ' ' )
            {
                *p = '\0';
                do
                    ++p;
                while( *p == ' ' );
                break;
            }
            ++p;
        }
        return start;
    };
    char* headers = strchr( p, '\n' );
    if( headers )
        *headers++ = '\0';
    else
        headers = const_cast<char*>( "" );

    char const* method = field();
    char const* path = field();
    char const* version = field();

    //LOG( "HTTP |" << method << '|' << path << '|' << version << '|' << strlen( headers ) );

    if( strcmp( method, "GET" ) == 0 )
    {
        /*
          Ignore any query parameters from paths like
          "/index.html?deeplink=com.bose.myApp&ssid=myNetwork&context=ApSetup".
          See https://github.com/BoseCorp/bose-web-riviera-ap-page/
        */
        if( auto q = strchr( path, '?' ) )
        {
            size_t len = q - path;
            if( send_static_content( std::string{ path, len } ) )
                _exit( EXIT_SUCCESS );
        }
        else
        {
            if( send_static_content( path ) )
                _exit( EXIT_SUCCESS );
        }
    }

    /* putenv keeps a pointer to these temporary std::string objects.
       This code assumes we execl and the temporary objects never actually
       get destroyed. */

    auto env1 = "REMOTE_ADDRESS=" + remote_address;
    if( putenv( const_cast< char* >( env1.c_str() ) ) != 0 )
        LOG( "putenv: " << err() );

    auto env2 = "LOCAL_ADDRESS=" + local_address;
    if( putenv( const_cast< char* >( env2.c_str() ) ) != 0 )
        LOG( "putenv: " << err() );

    auto env3 = "IFACE=" + iface;
    if( putenv( const_cast< char* >( env3.c_str() ) ) != 0 )
        LOG( "putenv: " << err() );

    execl( handler_program, handler_program,
           method, path, version, headers,
           nullptr );
    DIE( "execl " << handler_program << ": " << err() );
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
        auto remote_address = to_string( clientaddr, clientlen );
        std::string local_address;
        auto iface = get_iface_name( connfd, local_address );

        LOG( "accept " << remote_address << " on " << iface << ' ' << local_address );

        handle_connection( connfd, remote_address, local_address, iface );
        close( connfd );
    }
}

/* If the beginning of the string `target` equals the string `match` then
   advance the pointer `target` by the length of `match` and return true.
   Otherwise return false. */
bool skip_past( char* &target, char const* match )
{
    auto match_len = strlen( match );
    if( strncmp( target, match, match_len ) != 0 )
        return false;
    target += match_len;
    return true;
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
        char* arg = *++argv;

        // --static=FILE  Read the given file into the static content map.
        if( skip_past( arg, "--static=" ) )
        {
            read_static_content_map( arg );
            continue;
        }

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
