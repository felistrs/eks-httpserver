#include <assert.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>

#include "socket.h"

#include "../utils/dataBuffer.h"
#include "../utils/logger.h"


namespace server {


const ssize_t c_read_buffer_sz = 2048; // TODO: more/less

using socket_handler = int;


static std::map<connection_handler, connection_descriptor> g_connection_descriptors =
    std::map<connection_handler, connection_descriptor>();

static connection_handler g_handler_counter = 0;
static std::mutex g_lock_connection_descriptors;


std::pair<connection_handler, connection_descriptor*> GenerateConnectionHandler()
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    bool found = false;
    connection_handler handler = CONNECTION_HANDLER_INVALID;
    connection_descriptor* descr = nullptr;

    // Search for empty slot
    connection_handler h_start = g_handler_counter;

    for (;;)
    {
        ++g_handler_counter;

        if (g_handler_counter == CONNECTION_HANDLER_INVALID)
            ++g_handler_counter;

        if (g_handler_counter == h_start)
            break;

        auto it = g_connection_descriptors.find(g_handler_counter);
        if (it == g_connection_descriptors.end())
        {
            found = true;
            break;
        }
    }

    // New descriptor
    if (found)
    {
        handler = g_handler_counter;
        g_connection_descriptors[handler] = connection_descriptor();
        descr = &g_connection_descriptors[handler];
    }

    return std::make_pair(handler, descr);
}

void ReleaseConnectionHandler(connection_handler handler)
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    if (handler != CONNECTION_HANDLER_INVALID)
    {
        auto it = g_connection_descriptors.find(handler);
        if (it != g_connection_descriptors.end())
            g_connection_descriptors.erase(it);
    }
}

void ForEachConnection(std::function<void(connection_handler, connection_descriptor *)> call_back)
{
    for (auto& it : g_connection_descriptors)
    {
        connection_descriptor* descr = &it.second;
        call_back(it.first, descr);
    }
}

connection_descriptor* GetConnectionDescriptor(connection_handler handler)
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    auto it = g_connection_descriptors.find(handler);
    if (it != g_connection_descriptors.end())
        return &(it->second);
    else
    {
        error("Request for bad handler");
        return nullptr;
    }
}


connection_handler CreateConnectionForServer(int port)
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
                // unroll 2-nd (connection)
                ::shutdown(socket_status, 2);
            }
            // unroll 1-st (addr)
        }

        throw SocketException(err_info, sock_err, ::strerror(sock_err));
    }

    auto p = GenerateConnectionHandler();
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

        auto p = GenerateConnectionHandler();
        p.second->sock_handler = new_fd;
        result = p.first;
    }
    else {
        throw ConnectionException("AcceptNewConnection: handler is invalid");
    }

    return result;
}

void CloseConnection(int handler)
{
    auto descr = GetConnectionDescriptor(handler);
    if (descr)
    {
        ReleaseConnectionHandler(handler);  // TODO: bug, need lock

        int status = ::shutdown(descr->sock_handler, 2);

        if (status == -1)
        {
            auto err = errno;
            throw SocketException("Close connection failed", err, ::strerror(err));
        }
    }
    else {
        throw ConnectionException("CloseSocket: handle is invalid");
    }
}

void WriteBuffer(connection_handler handler, DataBuffer *buffer)
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
        throw ConnectionException("WriteBuffer: handler is invalid");
    }
}

DataBuffer ReadBuffer(connection_handler handler)
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
            throw SocketException("ReadBuffer failed", err, ::strerror(err));
        }

        if (read_sz)
        {
            return DataBuffer(move(read_buffer), read_sz);
        }
        else {
            return DataBuffer();
        }
    }
    else {
        throw ConnectionException("ReadBuffer: handler is invalid");
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


void ObtainIdleConnections(
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


}
