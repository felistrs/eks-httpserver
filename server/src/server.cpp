#include <cstring>
#include <algorithm>
#include <exception>
#include <iostream> // !!!

#include "server.h"

#include "utils/logger.h"
#include "socket/socket.h"

namespace server {


Server::~Server()
{
    if (is_running())
    {
        Stop();
    }
}

void Server::StartListening()
{
    _main_sock = sock::CreateConnectionForServer(_port);

    sock::StartListening(_main_sock, max_connections());

    _is_running = true;
}

void Server::Start()
{
    using namespace std;

    StartListening();
    log( "Listening... ." );

    // Starts new thread for communications
    _comm_thread = unique_ptr<thread>(
        new thread([](Server *server){
            server->OnCommunication();
        }, this) );

    log("Waiting ...");

    for (;;)
    {
        std::vector<connection_handler> conn_read;
        std::vector<connection_handler> conn { _main_sock };

        sock::ObtainIdleConnections(conn, &conn_read, nullptr, nullptr);

        if (conn_read.size() && conn_read[0] == _main_sock)  // TODO ???
        {
            auto new_sock = sock::AcceptNewConnection(_main_sock);
            log("Accepted : "+to_string(new_sock));

            {
                std::unique_lock<std::mutex> lock(_lock_connections);
                _thr_new_connections.push_back(new_sock);
            }
        }

        // TODO: remove next
//        this_thread::sleep_for(chrono::seconds(5));
//        log("Close ... ");
//        _thr_stop_server_flag = true;
//        break;

        // sleep
        this_thread::sleep_for(chrono::milliseconds(CListenSleepMS));
    }

    if(_comm_thread)
    {
        _comm_thread->join();
    }
}

void Server::Stop()
{
    if (_is_running)
    {
        StopCommunication();
        sock::CloseConnection(_main_sock);
        _main_sock = 0;
        _is_running = false;
    }
}

void Server::OnCommunication()
{
    using namespace std;

    while (true) {
        // Test new connections
        {
            unique_lock<mutex> lock(_lock_connections);

            for (auto connection_handler : _thr_new_connections) {
                _comm_connections.push_back(connection_handler);
                OnConnect(_comm_connections.back());
            }
            _thr_new_connections.clear();
        }

        // Communicate
        if (_comm_connections.size())
        {
            std::vector<connection_handler> conn_read, conn_write, conn_except;
            std::vector<connection_handler> sock_conn = _comm_connections;

            sock::ObtainIdleConnections(sock_conn, &conn_read, &conn_write, &conn_except);

            for (auto conn_handler : conn_read)
            {
                log(to_string(conn_handler) + " need communication");
                OnCommunication(conn_handler);
            }
        }

        // test need connection close
        if (_thr_stop_server_flag) {
            log("DoCommunication closing.");
            CloseAllConnections();
            break;
        }

        // sleep
        this_thread::sleep_for(chrono::milliseconds(CCommunicationSleepMS));
    }
}

void Server::OnCommunication(connection_handler conn)
{}

void Server::StopCommunication()
{
    if (_comm_connections.size() && !_comm_thread) {
        warning("Connections are opened, but communication thread is closed.");
    }

    if (_comm_thread)
    {
        _thr_stop_server_flag = true;
        _comm_thread->join();
        _comm_thread = nullptr;
    }
}

void Server::CloseAllConnections()
{
    for (auto conn_handler : _comm_connections)
    {
        sock::CloseConnection(conn_handler);
    }
    _comm_connections.clear();
}

void Server::CloseConnection(connection_handler conn) {
    auto it = std::find(_comm_connections.begin(), _comm_connections.end(), conn);
    if (it != _comm_connections.end())
        _comm_connections.erase(it);

    sock::CloseConnection(conn);
}


}
