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

        // Runnable for thread pool
        {
            // Schedule
            for (auto& runnable_ : _http_runners) {
                _communication_thread_pool->ScheduleExecutionRunnable(runnable_.get(), [this](Runnable *runnable) {
                    log("Runnable is done : " + std::to_string(runnable->get_id()));
                    this->MarkRunnerStatusToDone(runnable);
                });
            }

            ChangeRunnersStatusToInProgress(_http_runners);

            EraseCompletedRunners();
        }

        // exit ?
        if (_close_signal_recieved) {
            break;
        }

        // sleep
        this_thread::sleep_for(chrono::milliseconds(CListenSleepMS));
    }

    Stop();
}

void Server::Stop()
{
    if (_is_running)
    {
        CloseAllConnections();
        sock::CloseConnection(_main_sock);
        _main_sock = 0;
        _is_running = false;
    }
}

void Server::OnCommunication(connection_handler conn)
{}

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

void Server::onEvent(ProgramBreakEvent *e)
{
    log("Server::onEvent: Received ProgramBreakEvent");
    _close_signal_recieved = true;
}

void Server::ChangeRunnersStatusToInProgress(std::list<std::unique_ptr<Runnable>>& list)
{
    _http_runners_in_process.splice(
            _http_runners_in_process.begin(), list);
}

void Server::MarkRunnerStatusToDone(Runnable *runnable)
{
    _http_mark_as_done__safe.Push(runnable);
}

void Server::EraseCompletedRunners()
{
    auto list = _http_mark_as_done__safe.PopAll();

    while (list.size())
    {
        Runnable* item = list.front();
        list.pop();

        auto it = std::find_if(_http_runners_in_process.begin(), _http_runners_in_process.end(), [&item](const auto& i) {
            return i.get() == item;
        });

        if (it == _http_runners_in_process.end()) {
            throw ConnectionException("(Server::EraseCompletedRunners) Runner was not found : " +
                                              std::to_string(item->get_id()));
        }
        else {
            _http_runners_in_process.erase(it);
        }
    }
}

}
