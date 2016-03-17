#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "sockets/socket.h"
#include "commandProcessor.h"


namespace srv {


class Server {
public:
    Server();
    virtual ~Server();

    void StartAsync();
    void Stop();

    virtual void OnConnect(socket_t sock) = 0;

    void set_listening_port(int port);

    bool is_running() const;

    int max_connections() const;
    void set_max_connections(int max_conn);

    void set_command_processor(CommandProcessor* processor);


    // Communication thread
    void DoCommunication();  // TODO: should be moved to private ?

protected:
    // commands
    std::unique_ptr<CommandProcessor> _comm_processor;

private:
    void clean();

    void StartListening();
    void StartAcceptingConnections();

    void StopCommunication();
    void CloseAllConnections();


    int _port;

    bool _is_running = false;
    int _max_connections = 100;

    // socket
    socket_t _main_sock;

    //
    std::mutex _lock_connections;
    std::vector<socket_t> _thr_new_connections;

    // thread for communication
    std::shared_ptr<std::thread> _comm_thread;
    bool _thr_stop_server_flag = false;

    std::vector<socket_t> _comm_connections;
};


}

#endif
