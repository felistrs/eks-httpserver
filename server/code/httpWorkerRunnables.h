//
// Created by felistrs on 13.04.16.
//

#ifndef SERVER_HTTPWORKERRUNNABLES_H
#define SERVER_HTTPWORKERRUNNABLES_H

#include "socket/socketTypes.h"
#include "thread_things/runnable.h"


namespace server {

class HttpCommandProcessorInterface;

enum class HttpWorkerRunnableType { ENone, EReadRequest, EWriteResponse, ESendData };


class HttpServerReadRunnable : public Runnable {
public:
    virtual ~HttpServerReadRunnable() {}
};


class HttpServerWriteRunnable : public Runnable {
public:
    virtual ~HttpServerWriteRunnable() {}
};


class HttpServerReadRequestRunnable : public HttpServerReadRunnable {
public:
    virtual ~HttpServerReadRequestRunnable() {}

    HttpServerReadRequestRunnable(connection_handler connection) :
            _connection(connection)
    {}

    virtual void run();

private:
    connection_handler _connection;
};


class HttpServerWriteResponseRunnable : public HttpServerWriteRunnable {
public:
    virtual ~HttpServerWriteResponseRunnable() {}

    HttpServerWriteResponseRunnable(connection_handler connection, HttpCommandProcessorInterface *command_processor) :
        _connection(connection),
        _command_processor(command_processor)
    {}

    virtual void run();

private:
    connection_handler _connection;
    HttpCommandProcessorInterface *_command_processor;
};


class HttpServerSendDataRunnable : public HttpServerReadRunnable {
public:
    virtual ~HttpServerSendDataRunnable() {}

    virtual void run();
};


}

#endif //SERVER_HTTPWORKERRUNNABLES_H
