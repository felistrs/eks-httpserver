//
// Created by felistrs on 13.04.16.
//

#ifndef SERVER_HTTPWORKERRUNNABLES_H
#define SERVER_HTTPWORKERRUNNABLES_H

#include "serverRunnable.h"
#include "socket/socketTypes.h"
#include "thread_things/runnable.h"


namespace server {

class HttpCommandProcessorInterface;

enum class HttpWorkerRunnableType { ENone, EReadRequest, EWriteResponse, ESendData };



class HttpServerReadRequestRunnable : public ServerRunnable {
public:
    virtual ~HttpServerReadRequestRunnable() {}

    HttpServerReadRequestRunnable(connection_handler connection) :
        ServerRunnable(connection)
    {}

    virtual void run();
};


class HttpServerWriteResponseRunnable : public ServerRunnable {
public:
    virtual ~HttpServerWriteResponseRunnable() {}

    HttpServerWriteResponseRunnable(connection_handler connection, HttpCommandProcessorInterface *command_processor) :
        ServerRunnable(connection),
        _command_processor(command_processor)
    {}

    virtual void run();

private:
    HttpCommandProcessorInterface *_command_processor;
};


class HttpServerSendDataRunnable : public ServerRunnable {
public:
    HttpServerSendDataRunnable(connection_handler connection) :
        ServerRunnable(connection)
    {}

    virtual ~HttpServerSendDataRunnable() {}

    virtual void run();
};


}

#endif //SERVER_HTTPWORKERRUNNABLES_H
