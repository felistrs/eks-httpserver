#ifndef SERVER_H
#define SERVER_H

#include "sockets.h"

#include <iostream>
#include <string>
#include <vector>


class Server {
public:
    Server();
    virtual ~Server();

    void start();
    void set_listening_port(int port);
    void start_async();

    bool is_running();
    void stop();

    void on_connect(int conn_fd);

private:
    void clean();
    void log(const std::string msg) const;

    void start_listening();
    void start_accepting_connections();

private:
    bool _is_running = false;
    int _port;

    bool _verbose = true;

    // socket
    int _socket;
    std::vector<int> _connections;

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    addrinfo hints, *res;
    int new_fd;
};

#endif
