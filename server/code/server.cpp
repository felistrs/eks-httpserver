#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

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
    _main_sock = CreateConnectionForServer(_port);

    server::StartListening(_main_sock, max_connections());

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
        // Test for close
        {
            std::vector<connection_handler> conn_to_close;
            ForEachConnection([this, &conn_to_close](connection_handler handler, connection_descriptor *descr)
                              {
                                  if (this->TestConnectionNeedsClose(descr)) {
                                      conn_to_close.push_back(handler);
                                  }
                              });
            for (auto handler : conn_to_close) {
                CloseServerConnection(handler);
            }
        }

        // Test for incoming events
        std::vector<connection_handler> conn_read, conn_write, conn_except;
        std::vector<connection_handler> sock_conn = _comm_connections;
        sock_conn.push_back(_main_sock);

        ObtainIdleConnections(sock_conn, &conn_read, &conn_write, &conn_except);

        // Test connections for read
        for (auto conn_handler : conn_read)
        {
            if (conn_handler == _main_sock)
            {
                auto new_connection = AcceptNewConnection(_main_sock);
                log("Accepted : "+to_string(new_connection));

                _comm_connections.push_back(new_connection);
                OnConnect(_comm_connections.back());
            }
            else {
                log(to_string(conn_handler) + " need communication");
                OnRead(conn_handler);
            }
        }

        // Test connections for write
        for (auto conn_handler : conn_write)
        {
            if (conn_handler == _main_sock) {
                error("Main sock Need write");
            }
            else {
                log(to_string(conn_handler) + " need write");
                OnWrite(conn_handler);
            }
        }

        // Test connections for exceptions
        for (auto conn_handler : conn_except)
        {
            error("Need write : " + to_string(conn_handler));
        }

        // Runnable for thread pool
        {
            // Schedule
            for (auto& runnable_ : _runnables) {
                Runnable* runnable__ = dynamic_cast<Runnable*>(runnable_.get());
                _communication_thread_pool->ScheduleExecutionRunnable(runnable__, [this](Runnable *runnable) {
                    log("Runnable is done : " + std::to_string(runnable->get_id()));
                    this->MarkRunnerStatusToDone(runnable);
                });
            }

            MarkRunnersStatusToInProgress(_runnables);

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
        CloseConnection(_main_sock);
        _main_sock = 0;
        _is_running = false;
    }
}

void Server::CloseAllConnections()
{
    for (auto conn_handler : _comm_connections)
    {
        CloseConnection(conn_handler);
    }
    _comm_connections.clear();
}

void Server::CloseServerConnection(connection_handler handler) {
    auto it = std::find(_comm_connections.begin(), _comm_connections.end(), handler);
    if (it != _comm_connections.end())
        _comm_connections.erase(it);

    try {
        CloseConnection(handler);
    }
    catch (...) {
        error("(Server::CloseServerConnection) commnumication not closed : " +
              std::to_string(handler));
    }
}

DataBuffer Server::ReadBuffer(connection_handler conn) {
    auto buff = server::ReadBuffer(conn);

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

void Server::MarkRunnersStatusToInProgress(std::list<std::unique_ptr<ServerRunnable>> &list)
{
    _runnables_in_process.splice(
            _runnables_in_process.begin(), list);
}

void Server::MarkRunnerStatusToWaitExecution(Runnable *runnable)
{
    ServerRunnable* server_runnable = dynamic_cast<ServerRunnable*>(runnable);
    if (server_runnable) {
        server_runnable->OnBeforeScheduling();
    }
    else {
        throw ConnectionException("(Server::MarkRunnerStatusToWait) Bad runnable : " +
            std::to_string(runnable->get_id()));
    }
}

void Server::MarkRunnerStatusToDone(Runnable *runnable)
{
    ServerRunnable* server_runnable = dynamic_cast<ServerRunnable*>(runnable);
    if (server_runnable) {
        server_runnable->OnFinished();
        _runnable_mark_as_done__safe.Push(server_runnable);
    }
    else {
        throw ConnectionException("(Server::MarkRunnerStatusToDone) Bad runnable : " +
            std::to_string(runnable->get_id()));
    }
}

void Server::EraseCompletedRunners()
{
    auto list = _runnable_mark_as_done__safe.PopAll();

    while (list.size())
    {
        ServerRunnable* item = list.front();
        list.pop();

        auto it = std::find_if(_runnables_in_process.begin(), _runnables_in_process.end(), [&item](const auto& i) {
            return i.get() == item;
        });

        if (it == _runnables_in_process.end()) {
            throw ConnectionException("(Server::EraseCompletedRunners) Runner was not found : " +
                                              std::to_string(item->get_id()));
        }
        else {
            _runnables_in_process.erase(it);
        }
    }
}

}
