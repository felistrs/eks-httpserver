#include "server.h"
#include "socket_utils.h"

#include <exception>
#include <cstring>


Server::Server()
{
}

Server::~Server()
{
    if (is_running())
    {
        stop();
    }
}

void Server::clean()
{

}

void Server::log(const std::string msg) const
{
    if (_verbose)
    {
        std::cout << msg << std::endl;
    }
}

void Server::start_listening()
{
    // init socket
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(_port);
    int status;
    if ((status = getaddrinfo(NULL, port_str.c_str(), &hints, &res)) != 0)
    {
        std::string info = gai_strerror(status);
        throw SocketException(info);
    }

    _socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(_socket, res->ai_addr, res->ai_addrlen);

    // listen
    listen(_socket, max_connections());

    log("Listening port: " + std::to_string(_port));

    _is_running = true;
}

void Server::start_accepting_connections()
{
    socklen_t addr_size = sizeof their_addr;

    while (true)
    {
        log("Waiting ...");

        int new_fd;
        if ((new_fd = accept(_socket, (struct sockaddr *)&their_addr, &addr_size)) == -1)
        {
            std::string info = "Accept error : " + std::to_string(errno); // TODO: read string
            throw SocketException(info);
        }

        // Add new connection
        log("Connected.");

        _connections.push_back(new_fd);
        on_connect(new_fd); // TODO: make it in the thread ?

        break; // TODO: accept multiple clients
    }
}

void Server::start()
{
    start_listening();
    start_accepting_connections();
}

void Server::set_listening_port(int port)
{
    _port = port;
}

void Server::start_async()
{

}

bool Server::is_running()
{
    return _is_running;
}

void Server::stop()
{
    if (_is_running)
    {
//        close(_socket); // TODO: close connections !!!
        _socket = 0;

        for (auto conn : _connections)
        {
//            close(conn);
        }
        _connections.clear();

        _is_running = false; // TODO: _socket == 0 instead
    }
}

int Server::max_connections()
{
    return _max_connections;
}

void Server::set_max_connections(int max_conn)
{
    _max_connections = max_conn;
}
