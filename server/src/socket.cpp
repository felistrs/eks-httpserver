#include "sockets/socket.h"

#include <cstring>

#include <iostream>

#include "sockets/socket_utils.h"


namespace srv {


Socket::Socket()
{
    init();
}

Socket::Socket(int descriptor) :
    _descriptor(descriptor)
{
    init();
}

//Socket::Socket(const Socket &s)
//{
//    _descriptor = s._descriptor;
//}

//Socket &Socket::operator =(const Socket s)
//{
//    _descriptor = s._descriptor;
//}

void Socket::InitForLocalListening(int port)
{
    _port = port;

    // init socket
    std::memset(&_hints, 0, sizeof _hints);
    _hints.ai_family = AF_UNSPEC;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(port);
    int status;
    if ((status = getaddrinfo(NULL, port_str.c_str(), &_hints, &_tmp_res)) != 0)
    {
        std::string info = gai_strerror(status);
        throw SocketException(info);
    }

    _descriptor = socket(
                _tmp_res->ai_family,
                _tmp_res->ai_socktype,
                _tmp_res->ai_protocol);
    bind(_descriptor, _tmp_res->ai_addr, _tmp_res->ai_addrlen);
}

void Socket::StartListening(int max_connections)
{
    listen(_descriptor, max_connections);

    // log("Listening port: " + std::to_string(_port));
    std::cout << "Listening port: " << std::to_string(_port) << std::endl;
}

std::shared_ptr<Socket> Socket::Accept()
{
    int new_fd;

    socklen_t addr_size = sizeof _their_addr;

    if ((new_fd = accept(_descriptor, (struct sockaddr *)&_their_addr, &addr_size)) == -1)
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
    shutdown(_descriptor, 2);
}

void Socket::init()
{
    _read_buffer = std::vector<buff_t>(c_read_buffer_sz);
}

void Socket::read_buffer()
{
    ssize_t read_sz = recv(_descriptor, _read_buffer.data(),
                           c_read_buffer_sz, 0);
    std::cout << "Read buffer sz: " << read_sz << std::endl;

    _sbuffer.sputn(_read_buffer.data(), read_sz);

    _istream = std::make_shared<std::istream>(&_sbuffer);
}


}
