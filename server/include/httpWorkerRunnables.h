//
// Created by felistrs on 13.04.16.
//

#ifndef SERVER_HTTPWORKERRUNNABLES_H
#define SERVER_HTTPWORKERRUNNABLES_H


#include <cassert>

#include "httpThings.h"
#include "socket/socketFunctions.h"
#include "socket/socketTypes.h"
#include "thread_things/runnable.h"
#include "utils/dataBuffer.h"


namespace server {


class HttpServerDoResponseRunnable : public Runnable {
public:
    HttpServerDoResponseRunnable(connection_handler connection, HttpCommandProcessorInterface *command_processor) :
            _connection(connection),
            _command_processor(command_processor) { }

    virtual void run() {
        // Read request
        DataBuffer buff = Server::ReadBuffer(_connection);
        HttpRequest request = HttpServer::ReadRequest(&buff);

        //
        assert(_command_processor);
        HttpResponse response = _command_processor->ProcessRequest(&request);

        // Send responce
        auto buffer = response.Generate();
        sock::SendBuffer(_connection, &buffer);

        // change stat
        auto descr = GetConnectionDescriptor(_connection);

        if (response.DoCloseConnection()) {
            descr->state = HttpConnectionState::CNeedClose;
        }
        else {
            descr->state = HttpConnectionState::CDataSending;
        }
    }

private:
    connection_handler _connection;
    HttpCommandProcessorInterface *_command_processor;
};


class HttpServerSendDataRunnable : public Runnable {
public:
    virtual void run() {
        assert(false); // TODO: this
    }
};


}

#endif //SERVER_HTTPWORKERRUNNABLES_H
