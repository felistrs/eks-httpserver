#include "server.h"

#include <cstring>

#include <exception>

#include "sockets/socket_utils.h"


namespace srv {


Server::Server()
{}

Server::~Server()
{
    if (is_running())
    {
        Stop();
    }
}

void Server::clean()
{}

void Server::log(const std::string msg) const
{
    if (_verbose)
    {
        std::cout << msg << std::endl;
    }
}

void Server::StartListening()
{
    // init socket
    std::memset(&_hints, 0, sizeof _hints);
    _hints.ai_family = AF_UNSPEC;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(_port);
    int status;
    if ((status = getaddrinfo(NULL, port_str.c_str(), &_hints, &_tmp_res)) != 0)
    {
        std::string info = gai_strerror(status);
        throw SocketException(info);
    }

    _socket = socket(_tmp_res->ai_family, _tmp_res->ai_socktype, _tmp_res->ai_protocol);
    bind(_socket, _tmp_res->ai_addr, _tmp_res->ai_addrlen);

    // listen
    listen(_socket, max_connections());

    log("Listening port: " + std::to_string(_port));

    _is_running = true;
}

void Server::StartAcceptingConnections()
{
    socklen_t addr_size = sizeof _their_addr;

    while (true)
    {
        log("Waiting ...");

        int new_fd;
        if ((new_fd = accept(_socket, (struct sockaddr *)&_their_addr, &addr_size)) == -1)
        {
            std::string info = "Accept error : " + std::to_string(errno); // TODO: read string
            throw SocketException(info);
        }

        // Add new connection
        log("Connected.");

        _connections.push_back(new_fd);
        OnConnect(new_fd); // TODO: make it in the thread ?

        break; // TODO: accept multiple clients
    }
}

void Server::Start()
{
    StartListening();
    StartAcceptingConnections();
}

void Server::set_listening_port(int port)
{
    _port = port;
}

void Server::StartAsync()
{
    // TODO:
}

bool Server::is_running() const
{
    return _is_running;
}

void Server::Stop()
{
    if (_is_running)
    {
        for (auto conn : _connections)
        {
            shutdown(conn, 2);
        }
        _connections.clear();

        shutdown(_socket, 2);
        _socket = 0;

        _is_running = false; // TODO: _socket == 0 instead
    }
}

int Server::max_connections() const
{
    return _max_connections;
}

void Server::set_max_connections(int max_conn)
{
    _max_connections = max_conn;
}

}
