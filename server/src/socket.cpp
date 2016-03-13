#include "sockets/socket.h"

#include <cstring>

#include <chrono>
#include <iostream>
#include <thread>

#include "sockets/socket_utils.h"
#include "utils/logger.h"


namespace srv {


Socket::Socket()
{
    init();
}

Socket::Socket(socket_t descriptor) :
    _descriptor(descriptor)
{
    init();
}

socket_t Socket::SocketForLocalListening(int port)
{
    socket_t descriptor;

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

void Socket::StartListening(socket_t descriptor, int max_connections)
{
    if ( listen(descriptor, max_connections) < 0 )
    {
        throw SocketException("Listen failed");
    }

    log( "Listening... ." );
}

std::shared_ptr<Socket> Socket::Accept(socket_t descriptor)
{
    int new_fd;

    struct sockaddr_storage _their_addr;
    socklen_t addr_size = sizeof _their_addr;

    if ((new_fd = accept(descriptor, (struct sockaddr *)&_their_addr, &addr_size)) == -1)
    {
        std::string info = "Accept error : " + std::to_string(errno); // TODO: read string
        throw SocketException(info);
    }

    return std::shared_ptr<Socket>(new Socket(new_fd));
}

std::istream &Socket::InStream()
{
    if (!_istream) {
        read_buffer();
    }

    return *_istream;
}

void Socket::Close()
{
    Close(_descriptor);
}

void Socket::Close(int descriptor)
{
    shutdown(descriptor, 2);
}

void Socket::init()
{
    _read_buffer = std::vector<char>(c_read_buffer_sz);
}

void Socket::read_buffer()
{
    ssize_t read_sz = recv(_descriptor, _read_buffer.data(),
                           c_read_buffer_sz, 0);
    std::cout << "Read buffer sz: " << read_sz << std::endl;
    debug_hex(_read_buffer);
    debug_string(_read_buffer);
    std::cout << std::endl;

    _sbuffer.sputn(_read_buffer.data(), read_sz);

    _istream = std::make_shared<std::istream>(&_sbuffer);
}


}
