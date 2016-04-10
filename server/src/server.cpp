#include <cassert>
#include <cstring>
#include <algorithm>
#include <exception>
#include <iostream> // !!!

#include "server.h"

#include "utils/logger.h"
#include "socket/socket.h"
#include "thread_things/threadPool.h"


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

    _communication_thread_pool = CreateThreadPool();
    _communication_thread_pool->Start();
    log("Waiting ...");

    for (;;)
    {
        // Accept new connections
        std::vector<connection_handler> conn_read;
        std::vector<connection_handler> conn { _main_sock };

        sock::ObtainIdleConnections(conn, &conn_read, nullptr, nullptr);

        if (conn_read.size() && conn_read[0] == _main_sock)
        {
            auto new_connection = sock::AcceptNewConnection(_main_sock);
            log("Accepted : "+to_string(new_connection));

            _comm_connections.push_back(new_connection);
            OnConnect(_comm_connections.back());
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

        // Task for thread pool
        {
            for (auto& task : _http_tasks) {
                _communication_thread_pool->PushTask(task.get());
            }

            _http_tasks_in_process.splice(
                    _http_tasks_in_process.begin(), _http_tasks);
        }

        // sleep
        this_thread::sleep_for(chrono::milliseconds(CListenSleepMS));

//        break; // !!!
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

void Server::OnCommunication(connection_handler conn)
{}

void Server::StopCommunication()
{
    assert(false);
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
