#include <cstring>

#include <exception>
#include <iostream> // !!!

#include "server.h"

#include "utils/logger.h"
#include "socket/socket.h"


namespace server {


Server::Server()
{}

Server::~Server()
{
    if (is_running())
    {
        Stop();
    }
}

void Server::StartListening()
{
    _main_sock = sock::CreateSocketForServer(_port);
    sock::StartListening(_main_sock, max_connections());

    _is_running = true;
}

void Server::StartAsync()
{
    using namespace std;

    StartListening();

    // Starts new thread for communications
    _comm_thread = unique_ptr<thread>(
        new thread([](Server *server){
            server->OnCommunication();
        }, this) );

    log("Waiting ...");

    while (true)
    {
        std::vector<socket_handler> conn_read, conn_write, conn_except;
        std::vector<socket_handler> conn { _main_sock };

        sock::ObtainIdleSockets(conn, conn_read, conn_write, conn_except);

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
        sock::CloseSocket(_main_sock);
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

            for (socket_handler sock : _thr_new_connections) {
                connection_descriptor conn;
                conn.sock = sock;
                _comm_connections.push_back(conn);
                OnConnect(_comm_connections.back());
            }
            _thr_new_connections.clear();
        }

        // Communicate
        if (_comm_connections.size())
        {
            std::vector<socket_handler> conn_read, conn_write, conn_except;
            std::vector<socket_handler> sock_conn;
            for (const auto& conn : _comm_connections)
                sock_conn.push_back(conn.sock);

            sock::ObtainIdleSockets(sock_conn,
                                    conn_read, conn_write, conn_except);

            for (auto conn : conn_read)
            {
                log(to_string(conn) + " need communication");
                for (int i = 0; i < _comm_connections.size(); ++i) {  // TODO: with handler & descr.
                    if (_comm_connections[i].sock == conn) {
                        OnCommunication(_comm_connections[i]);
                        break;
                    }
                }

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

void Server::OnCommunication(connection_descriptor &conn)
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
    for (auto conn : _comm_connections)
    {
        sock::CloseSocket(conn.sock);
    }
    _comm_connections.clear();
}


}
