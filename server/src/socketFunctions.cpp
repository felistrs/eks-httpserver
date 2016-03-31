#include <cstring>

#include <chrono>
#include <iostream>
#include <thread>

#include "socket/socket.h"

#include "utils/logger.h"
#include "utils/buffer.h"


namespace server { namespace sock {


const ssize_t c_read_buffer_sz = 2048; // TODO: more/less


socket_handler CreateSocketForServer(int port)
{
    socket_handler descriptor = 0;

    bool all_is_ok = false;
    bool state_1_addr = false, state_2_socket = false,
         state_3_opt = false, state_4_bind = false;
    int addr_status, socket_status, opt_status, bind_status, sock_err;
    std::string err_info;


    do {
        // Get Addr Info
        addrinfo hints;
        addrinfo *addr;

        std::memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        std::string port_str = std::to_string(port);

        addr_status = ::getaddrinfo(NULL, port_str.c_str(), &hints, &addr);

        if (addr_status == -1)
        {
            sock_err = errno;
            err_info = "GetAddrInfo failed";
            break;
        }
        state_1_addr = true;

        // Socket
        socket_status = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (socket_status <= 0)
        {
            sock_err = errno;
            err_info = "Socket failed";
            break;
        }
        state_2_socket = true;

        // Change Options
        int opt = 1; // TRUE
        opt_status = ::setsockopt(socket_status, SOL_SOCKET, SO_REUSEADDR,
                       (char *)&opt, sizeof(opt));
        if (opt_status == -1)
        {
            sock_err = errno;
            err_info = "SetSockOpt failed";
            break;
        }
        state_3_opt = true;

        // Bind
        bind_status = ::bind(socket_status, addr->ai_addr, addr->ai_addrlen);
        if (bind_status == -1)
        {
            sock_err = errno;
            err_info = "Bind failed";
            break;
        }
        state_4_bind = true;

        // Everything is OK
        descriptor = socket_status;
        all_is_ok = true;
    } while (false);

    if (!all_is_ok)
    {
        if (state_1_addr) {
            if (state_2_socket) {
                if (state_3_opt) {
                    if (state_4_bind) {
                        // unroll 4-th (bind)
                    }
                    // unroll 3-rd (opt)
                }
                // unroll 2-nd (socket)
                ::shutdown(socket_status, 2);
            }
            // unroll 1-st (addr)
        }

        throw SocketException(err_info, sock_err, ::strerror(sock_err));
    }

    return descriptor;
}

void StartListening(socket_handler descriptor, int max_connections)
{
    int status = ::listen(descriptor, max_connections);

    if ( status == -1 )
    {
        auto err = errno;
        throw SocketException("Listen failed", err, ::strerror(err) );
    }
}

socket_handler AcceptNewConnection(socket_handler descriptor)
{
    struct sockaddr_storage _their_addr;
    socklen_t addr_size = sizeof _their_addr;

    socket_handler new_fd = ::accept(descriptor, (struct sockaddr *)&_their_addr, &addr_size);
    if (new_fd == -1)
    {
        auto err = errno;
        throw SocketException("Accept failed", err, ::strerror(err));
    }

    return new_fd;
}

void CloseSocket(int descriptor)
{
    int status = ::shutdown(descriptor, 2);

    if (status == -1)
    {
        auto err = errno;
        throw SocketException("Close socket failed", err, ::strerror(err));
    }
}

void SendBuffer(socket_handler sock, Buffer* buffer)
{
    using namespace std;

    ssize_t needed_sz = buffer->size();
    ssize_t res = ::send(sock, buffer->data().data(), needed_sz, 0);

    if (res == -1)
    {
        auto err = errno;
        string info = err == EMSGSIZE ?
                    "Send data is too large" : "Send failed";
        throw SocketException(info, err, ::strerror(err));
    }
    else if (res != buffer->size())
    {
        warning("Sent " + to_string(res) +
                " instead of " + to_string(needed_sz) );
    }
}

Buffer RecvBuffer(socket_handler sock)
{
    using namespace std;

    vector<char> read_buffer(c_read_buffer_sz);

    ssize_t read_sz = ::recv(sock, read_buffer.data(),
                           c_read_buffer_sz, 0);

    if (read_sz == -1)
    {
        auto err = errno;
        throw SocketException("RecvBuffer failed", err, ::strerror(err));
    }

    if (read_sz)
    {
        return Buffer(move(read_buffer), read_sz);
    }
    else {
        return Buffer();
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

    int status = ::select(max_fd + 1, read_fd, write_fd, except_fd, &timeout);

    if (status == -1)
    {
        auto err = errno;
        if (err != EINTR) // signal was captured
        {
            throw SocketException("Select failed", err, ::strerror(err));
        }
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
        exception_out = ObtainIdleSocketsFor(SelectType::Exception, connections, 0);
    }
    else
    {
        read_out.clear();
        write_out.clear();
        exception_out.clear();
    }
}


} }
