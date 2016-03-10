#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <vector>

#include "sockets/socket.h"


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

    virtual void OnConnect(std::shared_ptr<Socket> sock) = 0;

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
    std::shared_ptr<Socket> _main_sock;

//    int _socket = 0;
    std::vector<std::shared_ptr<Socket>> _connections;
};


}

#endif
