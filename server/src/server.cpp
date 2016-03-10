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
    _main_sock = std::shared_ptr<Socket>(new Socket);

    _main_sock->InitForLocalListening(_port); // TODO: not local.
    _main_sock->StartListening( max_connections() );

    _is_running = true;
}

void Server::StartAcceptingConnections()
{
    while (true)
    {
        log("Waiting ...");

        auto new_sock = _main_sock->Accept();

        // Add new connection
        log("Connected.");

        _connections.push_back(new_sock);
        OnConnect(new_sock); // TODO: make it in the thread ?

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
        for (auto sock : _connections)
        {
            sock->Close();
        }
        _connections.clear();

        _main_sock->Close();

        _is_running = false;
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
