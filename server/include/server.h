#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <vector>

#include "sockets/sockets.h"


class Server {
public:
    Server();
    virtual ~Server();

    void start();
    void set_listening_port(int port);
    void start_async();

    bool is_running();
    void stop();

    virtual void on_connect(int conn_fd) = 0;

    int max_connections();
    void set_max_connections(int max_conn);

private:
    void clean();
    void log(const std::string msg) const;

    void start_listening();
    void start_accepting_connections();

private:
    int _port;

    bool _verbose = true;

    bool _is_running = false;
    int _max_connections = 100;

    // socket
    int _socket = 0;
    std::vector<int> _connections;

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    addrinfo hints, *res;
    int new_fd;
};

#endif
