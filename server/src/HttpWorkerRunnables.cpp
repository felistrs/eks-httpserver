//
// Created by felistrs on 15.04.16.
//

#include <cassert>

#include "httpCommandProcessorInterface.h"
#include "httpServer.h"
#include "httpThings.h"
#include "httpWorkerRunnables.h"
#include "socket/socketFunctions.h"
#include "socket/socketTypes.h"
#include "thread_things/runnable.h"
#include "utils/dataBuffer.h"


namespace server {


void HttpServerDoResponseRunnable::run() {
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


void HttpServerSendDataRunnable::run() {
    assert(false); // TODO: this
}


}