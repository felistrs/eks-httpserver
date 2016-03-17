#include "server.h"

#include <cstring>

#include <exception>

#include "sockets/socket_utils.h"
#include "utils/logger.h"


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

void Server::StartListening()
{
    _main_sock = sock::SocketForLocalListening(_port);
    sock::StartListening(_main_sock, max_connections());

    _is_running = true;
}

void Server::StartAsync()
{
    using namespace std;

    StartListening();

    fd_set main_set_fd;
    FD_ZERO(&main_set_fd); // clear set
    FD_SET(_main_sock, &main_set_fd); // add socket
    socket_t max_fd = _main_sock;

    int activity_res;

    // Starts new thread for communications
    _comm_thread = shared_ptr<thread>(
        new thread([](Server *server){
            server->OnCommunication();
        }, this) );

    log("Waiting ...");

    while (true)
    {
        activity_res = select( max_fd + 1 , &main_set_fd, NULL , NULL , NULL);

        if ((activity_res < 0) && (errno!=EINTR))
        {
            throw SocketException("(Server::StartAsync) Select error;");
        }

        // something with main socket
        if (FD_ISSET(_main_sock, &main_set_fd))
        {
            auto new_sock = sock::Accept(_main_sock);
            log("Accepted : "+to_string(new_sock));

            {
                std::unique_lock<std::mutex> lock(_lock_connections);
                _thr_new_connections.push_back(new_sock);

//                OnConnect(new_sock); // !!!
//                this_thread::sleep_for(chrono::seconds(5));
//                log("Close ... ");
//                _thr_stop_server_flag = true;
//                break;
            }
        }

        // TODO: remove next
        this_thread::sleep_for(chrono::seconds(5));
        log("Close ... ");
        _thr_stop_server_flag = true;
        break;
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
        sock::Close(_main_sock);
        _main_sock = 0;
        _is_running = false;
    }
}

void Server::set_listening_port(int port)
{
    _port = port;
}

bool Server::is_running() const
{
    return _is_running;
}

int Server::max_connections() const
{
    return _max_connections;
}

void Server::set_max_connections(int max_conn)
{
    _max_connections = max_conn;
}

void Server::set_command_processor(CommandProcessor* processor)
{
    _comm_processor = std::unique_ptr<CommandProcessor>(processor);
}

std::shared_ptr<Buffer> Server::ReadBuffer(socket_t sock)
{
    using namespace std;

    shared_ptr<Buffer> res;

    const ssize_t c_read_buffer_sz = 2048; // TODO: more/less
    vector<char> read_buffer(c_read_buffer_sz);

    ssize_t read_sz = recv(sock, read_buffer.data(),
                           c_read_buffer_sz, 0);

    if (read_sz)
    {
        res = shared_ptr<Buffer>(new Buffer(read_buffer, read_sz));
    }

    return res;
}

void Server::OnCommunication()
{
    using namespace std;

    while (true) {
        // Test new connections
        {
            unique_lock<mutex> lock(_lock_connections);

            for (socket_t sock : _thr_new_connections) {
                conn_t conn;
                conn.sock = sock;
                _comm_connections.push_back(conn);
            }
            _thr_new_connections.clear();
        }

        // Communicate
        if (_comm_connections.size())
        {
            fd_set comm_set_fd;
            FD_ZERO(&comm_set_fd); // clear set
            socket_t max_fd = _comm_connections[0].sock;
            for (conn_t conn : _comm_connections)
            {
                FD_SET(conn.sock, &comm_set_fd); // add connection to set
                if (conn.sock > max_fd)
                    max_fd = conn.sock;
            }

            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int activity_res = select( max_fd + 1 , &comm_set_fd,
                                       NULL , NULL , &timeout);

            if ((activity_res < 0) && (errno!=EINTR))
            {
                throw SocketException("(Server::DoCommunication) Select error;");
            }

            for (conn_t& conn: _comm_connections)
            {
                if (FD_ISSET(conn.sock, &comm_set_fd))
                {
                    log(to_string(conn.sock) + " need communication");
                    OnCommunication(conn);
                }
            }
        }

        // test need connection close
        if (_thr_stop_server_flag) {
            log("DoCommunication closing.");
            CloseAllConnections();
            break;
        }
    }
}

void Server::OnCommunication(Server::conn_t &conn)
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
        sock::Close(conn.sock);
    }
    _comm_connections.clear();
}


}
