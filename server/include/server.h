#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <vector>

#include "sockets/socket.h"
#include "commandProcessor.h"


namespace srv {


class Server {
public:
    Server();
    virtual ~Server();

    void Start();
    void set_listening_port(int port);
    void StartAsync(); // TODO: !!!

    bool is_running() const;
    void Stop();

    virtual void OnConnect(std::shared_ptr<Socket> sock) = 0;

    int max_connections() const;
    void set_max_connections(int max_conn);

    void set_command_processor(CommandProcessor* processor);

protected:
    // commands
    std::unique_ptr<CommandProcessor> _comm_processor;

private:
    void clean();

    void StartListening();
    void StartAcceptingConnections();


    int _port;

    bool _is_running = false;
    int _max_connections = 100;

    // socket
    int _main_sock;
    std::vector<std::shared_ptr<Socket>> _connections;

};


}

#endif
