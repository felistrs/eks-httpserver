#include <cstring>
#include <assert.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "socket/socket.h"

#include "utils/logger.h"
#include "utils/buffer.h"


namespace server { namespace sock {


const ssize_t c_read_buffer_sz = 2048; // TODO: more/less

using socket_handler = int;


connection_handler CreateSocketForServer(int port)
{
    socket_handler sock_descriptor = 0;

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
        sock_descriptor = socket_status;
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

    auto p = generateConnectionHandler();
    p.second->sock_handler = sock_descriptor;
    return p.first;
}

void StartListening(connection_handler handler, int max_connections)
{
    auto descr = GetConnectionDescriptor(handler);
    if (descr)
    {
        int status = ::listen(descr->sock_handler, max_connections);

        if ( status == -1 )
        {
            auto err = errno;
            throw SocketException("Listen failed", err, ::strerror(err) );
        }
    }
    else {
        throw ConnectionException("StartListening: handler is invalid");
    }
}

connection_handler AcceptNewConnection(connection_handler handler)
{
    connection_handler result = CONNECTION_HANDLER_INVALID;
    auto descr = GetConnectionDescriptor(handler);

    if (descr)
    {
        struct sockaddr_storage _their_addr;
        socklen_t addr_size = sizeof _their_addr;

        connection_handler new_fd = ::accept(descr->sock_handler,
                (struct sockaddr *)&_their_addr, &addr_size);
        if (new_fd == -1)
        {
            auto err = errno;
            throw SocketException("Accept failed", err, ::strerror(err));
        }

        auto p = generateConnectionHandler();
        p.second->sock_handler = new_fd;
        result = p.first;
    }
    else {
        throw ConnectionException("AcceptNewConnection: handler is invalid");
    }

    return result;
}

void CloseSocket(int handler)
{
    auto descr = GetConnectionDescriptor(handler);
    if (descr)
    {
        releaseConnectionHandler(handler);  // TODO: bug, need lock

        int status = ::shutdown(descr->sock_handler, 2);

        if (status == -1)
        {
            auto err = errno;
            throw SocketException("Close socket failed", err, ::strerror(err));
        }
    }
    else {
        throw ConnectionException("CloseSocket: handle is invalid");
    }
}

void SendBuffer(connection_handler handler, Buffer* buffer)
{
    using namespace std;

    auto descr = GetConnectionDescriptor(handler);
    if (descr)
    {
        ssize_t needed_sz = buffer->size();
        ssize_t res = ::send(descr->sock_handler,
                             buffer->data().data(), needed_sz, 0);

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
    else {
        throw ConnectionException("SendBuffer: handler is invalid");
    }
}

Buffer RecvBuffer(connection_handler handler)
{
    using namespace std;

    auto descr = GetConnectionDescriptor(handler);

    if (descr)
    {
        vector<char> read_buffer(c_read_buffer_sz);

        ssize_t read_sz = ::recv(descr->sock_handler,
                read_buffer.data(), c_read_buffer_sz, 0);

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
    else {
        throw ConnectionException("RecvBuffer: handler is invalid");
    }
}


namespace {

enum class SelectType {
    Read, Write, Exception
};

void ObtainIdleSocketsFor(
        SelectType type,
        const std::vector<connection_handler> &connections_in,
        std::vector<connection_handler>* connection_out,
        unsigned microsec_delay)
{
    assert(connection_out);
    connection_out->clear();

    if (connections_in.size())
    {
        // Connection handler vs Socket handler
        std::map<connection_handler, int> conn2sock;
        std::map<int, connection_handler> sock2conn;
        for (const auto& conn_h : connections_in)
        {
            auto descr = GetConnectionDescriptor(conn_h);
            if (descr)
            {
                conn2sock[conn_h] = descr->sock_handler;
                sock2conn[descr->sock_handler] = conn_h;
            }
            else {
                throw ConnectionException("ObtainIdleSocketsFor: invalid handler");
            }
        }


        fd_set set_fd;
        FD_ZERO(&set_fd); // clear set
        int max_fd = conn2sock.begin()->second;
        for (const auto& pair : conn2sock)
        {
            int sock = pair.second;
            FD_SET(sock, &set_fd); // add connection to set
            if (sock > max_fd)
                max_fd = sock;
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

        for (const auto& conn: conn2sock)
        {
            int sock = conn.second;
            if (FD_ISSET(sock, &set_fd))
            {
                connection_out->push_back(conn.first);
            }
        }
    } // connections.size()

}

}


void ObtainIdleSockets(
        const std::vector<connection_handler> &connections,
        std::vector<connection_handler> *read_out,
        std::vector<connection_handler> *write_out,
        std::vector<connection_handler> *exception_out)
{
    if (connections.size())
    {
        if (read_out)
            ObtainIdleSocketsFor(SelectType::Read,
                    connections, read_out, 0);

        if (write_out)
            ObtainIdleSocketsFor(SelectType::Write,
                    connections, write_out, 0);

        if (exception_out)
            ObtainIdleSocketsFor(SelectType::Exception,
                    connections, exception_out, 0);
    }
    else
    {
        if (read_out)
            read_out->clear();

        if (write_out)
            write_out->clear();

        if (exception_out)
            exception_out->clear();
    }
}


} }
