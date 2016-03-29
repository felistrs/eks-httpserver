#include <cstring>

#include <chrono>
#include <iostream>
#include <thread>

#include "socket/socket.h"

#include "utils/logger.h"
#include "utils/buffer.h"


namespace server { namespace sock {


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
    if ((status = ::getaddrinfo(NULL, port_str.c_str(), &hints, &tmp_res)) != 0)
    {
        std::string info = ::gai_strerror(status);
        throw SocketException(info);
    }

    descriptor = ::socket(
                tmp_res->ai_family,
                tmp_res->ai_socktype,
                tmp_res->ai_protocol);
    // TODO: delete tmp_res ?

    if (descriptor == 0)
    {
        throw SocketException("Main socket was not created.");
    }

    int opt = 1; // TRUE
    if( ::setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&opt, sizeof(opt)) < 0 )
    {
        throw SocketException("Not set option for multiple listeners.");
    }

    if ( ::bind(descriptor, tmp_res->ai_addr, tmp_res->ai_addrlen) < 0)
    {
        throw SocketException("Bind failed");
    }

    return descriptor;
}

void StartListening(socket_handler descriptor, int max_connections)
{
    if ( ::listen(descriptor, max_connections) < 0 )
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

    if ((new_fd = ::accept(descriptor, (struct sockaddr *)&_their_addr, &addr_size)) == -1)
    {
        std::string info = "Accept error : " + std::to_string(errno); // TODO: read string
        throw SocketException(info);
    }

    return new_fd;
}

void CloseSocket(int descriptor)
{
    ::shutdown(descriptor, 2);
}

void SendBuffer(socket_handler sock, Buffer* buffer)
{
    ssize_t res = ::send(sock, buffer->data().data(), buffer->size(), 0);

    if (res == ENOTCONN)
    {
        throw SocketException("(Server::SendBuffer) : socket is not connected");
    }
    else if (res == EMSGSIZE)
    {
        throw SocketException("(Server::SendBuffer) : send data is too large");
    }
    else // if (res != buffer->size())
    {
        //throw SocketException("(Server::SendBuffer) : " + );
        warning("send : " + std::to_string(res) );
    }
}

Buffer RecvBuffer(socket_handler sock)
{
    using namespace std;

    Buffer res;

    const ssize_t c_read_buffer_sz = 2048; // TODO: more/less
    vector<char> read_buffer(c_read_buffer_sz);

    ssize_t read_sz = ::recv(sock, read_buffer.data(),
                           c_read_buffer_sz, 0);

    if (read_sz)
    {
        return Buffer(std::move(read_buffer), read_sz);
    }
    else {
        return res;
    }
}


namespace {

enum class SelectType {
    Read, Write, Exception
};

std::vector<socket_handler> ObtainIdleSocketsFor(
        SelectType type,
        const std::vector<socket_handler> &connections,
        unsigned microsec_delay)
{
    std::vector<socket_handler> socks;

    fd_set set_fd;
    FD_ZERO(&set_fd); // clear set
    socket_handler max_fd = connections[0];
    for (const auto& conn : connections)
    {
        FD_SET(conn, &set_fd); // add connection to set
        if (conn> max_fd)
            max_fd = conn;
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = microsec_delay;

    fd_set* read_fd = (type == SelectType::Read) ? &set_fd : NULL;
    fd_set* write_fd = (type == SelectType::Write) ? &set_fd : NULL;
    fd_set* except_fd = (type == SelectType::Exception) ? &set_fd : NULL;

    int activity_res = ::select(max_fd + 1, read_fd,
                               write_fd, except_fd, &timeout);

    if ((activity_res < 0) && (errno!=EINTR))
    {
        throw SocketException("(Server::DoCommunication) Select error;");
    }

    for (const auto& conn: connections)
    {
        if (FD_ISSET(conn, &set_fd))
        {
            socks.push_back(conn);
        }
    }

    return socks;
}

}


void ObtainIdleSockets(
        const std::vector<socket_handler> &connections,
        std::vector<socket_handler> &read_out,
        std::vector<socket_handler> &write_out,
        std::vector<socket_handler> &exception_out)
{
    if (connections.size())
    {
        read_out = ObtainIdleSocketsFor(SelectType::Read, connections, 0);
        write_out = ObtainIdleSocketsFor(SelectType::Write, connections, 0);
        exception_out = ObtainIdleSocketsFor(SelectType::Exception, connections, 1000);
    }
    else
    {
        read_out.clear();
        write_out.clear();
        exception_out.clear();
    }
}


} }
