#ifndef SERVER_H
#define SERVER_H

#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "socket/socket.h"
#include "thread_things/threadPool.h"


class DataBuffer;


namespace server {


class Server {
public:
    Server() {}
    virtual ~Server();

    void Start();
    void Stop();

    static DataBuffer ReadBuffer(connection_handler conn);


public:
    void set_listening_port(int port) { _port = port; }
    bool is_running() const { return _is_running; }

    int max_connections() const { return _max_connections; }
    void set_max_connections(int max_conn) { _max_connections = max_conn; }


protected:
    const unsigned CListenSleepMS = 50;
    const unsigned CCommunicationSleepMS = 50;


    virtual void OnConnect(connection_handler conn) = 0;
    virtual void OnCommunication(connection_handler conn) = 0;
    virtual void OnDisconnect(connection_handler conn) = 0;

    virtual std::unique_ptr<ThreadPool> CreateThreadPool() = 0;

    void CloseConnection(connection_handler handler);


    std::list<std::unique_ptr<ThreadTask>> _http_tasks;
    std::list<std::unique_ptr<ThreadTask>> _http_tasks_in_process;

private:
    void StartListening();

    void StopCommunication();
    void CloseAllConnections();

    void EraseCompletedTasks();


    int _port;

    bool _is_running = false;
    int _max_connections = 100;

    // connections
    connection_handler _main_sock;
    std::vector<connection_handler> _comm_connections;

    // thread for communication
    std::unique_ptr<ThreadPool> _communication_thread_pool;

};


}

#endif
