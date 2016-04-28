//
// Created by felistrs on 15.04.16.
//

#include <cassert>

#include "httpCommandProcessorInterface.h"
#include "httpServer.h"
#include "httpThings.h"
#include "httpWorkerRunnables.h"
#include "connection/functions.h"
#include "connection/types.h"
#include "thread_things/runnable.h"
#include "utils/dataBuffer.h"


namespace server {


void HttpServerReadRequestRunnable::run() {
    // Read request
    DataBuffer buff = Server::ReadBuffer(_connection);

    HttpRequest* http_request = new HttpRequest;
    std::unique_ptr<communication_type> request(http_request);

    HttpServer::ReadRequest(&buff, *http_request);

    auto descr = GetConnectionDescriptor(_connection);
    if (descr) {
        descr->request = std::move(request);
        descr->state = HttpConnectionState::CNeedResponse;
    } else {
        throw  ConnectionException("(HttpServerReadRequestRunnable::run) : Bad descriptor;");
    }
}


void HttpServerWriteResponseRunnable::run() {
    assert(_command_processor);

    log("Writing response for :" + _connection);

    auto descr = GetConnectionDescriptor(_connection);
    if (descr == nullptr) {
        throw  ConnectionException("(HttpServerReadRequestRunnable::run) : Bad descriptor;");
    }

    HttpRequest* request = dynamic_cast<HttpRequest*>(descr->request.get());
    if (request == nullptr) {
        throw  ConnectionException("(HttpServerReadRequestRunnable::run) : Empty request;");
    }

    HttpResponse response = _command_processor->ProcessRequest(request);

    // Send responce
    auto buffer = response.Generate();
    WriteBuffer(_connection, &buffer);

    // change state
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
