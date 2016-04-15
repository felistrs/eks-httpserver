#ifndef SERVER_H
#define SERVER_H

#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "socket/socket.h"
#include "thread_things/queueSafe.h"
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


    QueueSafe<Runnable*> _http_mark_as_done__safe;

    std::list<std::unique_ptr<Runnable>> _http_runners;
    std::list<std::unique_ptr<Runnable>> _http_runners_in_process;


private:
    void StartListening();

    void StopCommunication();
    void CloseAllConnections();

    void ChangeRunnersStatusToInProgress(std::list<std::unique_ptr<Runnable>>& list);
    void MarkRunnerStatusToDone(Runnable *runnable);


    int _port;

    bool _is_running = false;
    int _max_connections = 100;

    // connections
    connection_handler _main_sock;
    std::vector<connection_handler> _comm_connections;

    // thread for communication
    std::unique_ptr<ThreadPool> _communication_thread_pool;

    void EraseCompletedRunners();
};


}

#endif
