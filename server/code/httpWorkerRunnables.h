//
// Created by felistrs on 13.04.16.
//

#ifndef SERVER_HTTPWORKERRUNNABLES_H
#define SERVER_HTTPWORKERRUNNABLES_H

#include "socket/socketTypes.h"
#include "thread_things/runnable.h"


namespace server {

class HttpCommandProcessorInterface;

enum class HttpWorkerRunnableType { ENone, EDoResponse, ESendData };


class HttpServerDoResponseRunnable : public Runnable {
public:
    virtual ~HttpServerDoResponseRunnable() {}

    HttpServerDoResponseRunnable(connection_handler connection, HttpCommandProcessorInterface *command_processor) :
            _connection(connection),
            _command_processor(command_processor) {}

    virtual void run();

private:
    connection_handler _connection;
    HttpCommandProcessorInterface *_command_processor;
};


class HttpServerSendDataRunnable : public Runnable {
public:
    virtual ~HttpServerSendDataRunnable() {}

    virtual void run();
};


}

#endif //SERVER_HTTPWORKERRUNNABLES_H
