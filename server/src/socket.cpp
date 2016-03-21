#include "sockets/socket.h"

#include <cstring>

#include <chrono>
#include <iostream>
#include <thread>

#include "utils/logger.h"


namespace server {
namespace sock {


socket_handler CreateSocketForServer(int port)
{
    socket_handler descriptor;

    addrinfo hints;
    addrinfo  *tmp_res;

    // init socket
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port);
    int status;
    if ((status = getaddrinfo(NULL, port_str.c_str(), &hints, &tmp_res)) != 0)
    {
        std::string info = gai_strerror(status);
        throw SocketException(info);
    }

    descriptor = socket(
                tmp_res->ai_family,
                tmp_res->ai_socktype,
                tmp_res->ai_protocol);
    // TODO: delete tmp_res ?

    if (descriptor == 0)
    {
        throw SocketException("Main socket was not created.");
    }

    int opt = 1; // TRUE
    if( setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&opt, sizeof(opt)) < 0 )
    {
        throw SocketException("Not set option for multiple listeners.");
    }

    if ( bind(descriptor, tmp_res->ai_addr, tmp_res->ai_addrlen) < 0)
    {
        throw SocketException("Bind failed");
    }

    return descriptor;
}

void StartListening(socket_handler descriptor, int max_connections)
{
    if ( listen(descriptor, max_connections) < 0 )
    {
        throw SocketException("Listen failed");
    }

    log( "Listening... ." );
}

socket_handler AcceptNewConnection(socket_handler descriptor)
{
    socket_handler new_fd;

    struct sockaddr_storage _their_addr;
    socklen_t addr_size = sizeof _their_addr;

    if ((new_fd = accept(descriptor, (struct sockaddr *)&_their_addr, &addr_size)) == -1)
    {
        std::string info = "Accept error : " + std::to_string(errno); // TODO: read string
        throw SocketException(info);
    }

    return new_fd;
}

void CloseSocket(int descriptor)
{
    shutdown(descriptor, 2);
}


}
}
