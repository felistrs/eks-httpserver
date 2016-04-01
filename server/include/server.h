#ifndef SERVER_H
#define SERVER_H

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "socket/socket.h"
#include "utils/buffer.h"


namespace server {


class Server {
public:
    Server();
    virtual ~Server();


    void StartAsync();
    void Stop();

    virtual void OnConnect(connection_descriptor& conn) = 0;
    virtual void OnCommunication(connection_descriptor& conn) = 0;
    virtual void OnDisconnect(connection_descriptor& conn) = 0;

public:
    void set_listening_port(int port) { _port = port; }
    bool is_running() const { return _is_running; }

    int max_connections() const { return _max_connections; }
    void set_max_connections(int max_conn) { _max_connections = max_conn; }


protected:
    const unsigned CListenSleepMS = 50;
    const unsigned CCommunicationSleepMS = 50;


private:
    void StartListening();

    void OnCommunication();
    void StopCommunication();
    void CloseAllConnections();


    int _port;

    bool _is_running = false;
    int _max_connections = 100;

    // socket
    connection_handler _main_sock;

    //
    std::mutex _lock_connections;
    std::vector<connection_handler> _thr_new_connections;

    // thread for communication
    std::unique_ptr<std::thread> _comm_thread;
    bool _thr_stop_server_flag = false;

    std::vector<connection_descriptor> _comm_connections;

};


}

#endif
