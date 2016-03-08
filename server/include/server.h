#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <vector>

#include "sockets/sockets.h"

namespace srv {


class Server {
public:
    Server();
    virtual ~Server();

    void Start();
    void set_listening_port(int port);
    void StartAsync();

    bool is_running() const;
    void Stop();

    virtual void OnConnect(int conn_fd) = 0;

    int max_connections() const;
    void set_max_connections(int max_conn);

private:
    void clean();
    void log(const std::string msg) const;

    void StartListening();
    void StartAcceptingConnections();


    int _port;

    bool _verbose = true;

    bool _is_running = false;
    int _max_connections = 100;

    // socket
    int _socket = 0;
    std::vector<int> _connections;

    struct sockaddr_storage _their_addr;
    socklen_t _addr_size;
    addrinfo _hints, *_tmp_res;
};


}

#endif
