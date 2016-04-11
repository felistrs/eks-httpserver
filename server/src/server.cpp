#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream> // !!!

#include "server.h"

#include "utils/logger.h"
#include "utils/dataBuffer.h"


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
        // Test for incoming events
        std::vector<connection_handler> conn_read, conn_write, conn_except;
        std::vector<connection_handler> sock_conn = _comm_connections;
        sock_conn.push_back(_main_sock);

        sock::ObtainIdleConnections(sock_conn, &conn_read, &conn_write, &conn_except);

        // Test connections for read
        for (auto conn_handler : conn_read)
        {
            if (conn_handler == _main_sock)
            {
                auto new_connection = sock::AcceptNewConnection(_main_sock);
                log("Accepted : "+to_string(new_connection));

                _comm_connections.push_back(new_connection);
                OnConnect(_comm_connections.back());
            }
            else {
                log(to_string(conn_handler) + " need communication");
                OnCommunication(conn_handler);
            }
        }

        // Test connections for write
        for (auto conn_handler : conn_write)
        {
            error("Need write : " + to_string(conn_handler));
        }

        // Test connections for exceptions
        for (auto conn_handler : conn_except)
        {
            error("Need write : " + to_string(conn_handler));
        }

        // Task for thread pool
        {
            for (auto& task : _http_tasks) {
                _communication_thread_pool->ScheduleExecutionTask(task.get());
            }

            _http_tasks_in_process.splice(
                    _http_tasks_in_process.begin(), _http_tasks);

            EraseCompletedTasks();
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

void Server::CloseConnection(connection_handler handler) {
    auto it = std::find(_comm_connections.begin(), _comm_connections.end(), handler);
    if (it != _comm_connections.end())
        _comm_connections.erase(it);

    try {
        sock::CloseConnection(handler);
    }
    catch (...) {
        error("(Server::CloseConnection) commnumication not closed : " +
              std::to_string(handler));
    }
}

DataBuffer Server::ReadBuffer(connection_handler conn) {
    auto buff = sock::RecvBuffer(conn);

    std::cout << "Read buffer sz: " << buff.data().size() << std::endl;
//    debug_hex(buff.data());
    debug_string(buff.data());
    std::cout << std::endl;

    return buff;
}

void Server::EraseCompletedTasks() {
    std::queue<ThreadTask*> completed_tasks = _communication_thread_pool->PopCompletedTasks();

    while (completed_tasks.size())
    {
        ThreadTask* task = completed_tasks.front();
        completed_tasks.pop();

        for (auto it = _http_tasks_in_process.begin(); it != _http_tasks_in_process.end(); ++it)
        {
            if (it->get() == task)
            {
                _http_tasks_in_process.erase(it);
                break;
            }
        }
    }
}


}
