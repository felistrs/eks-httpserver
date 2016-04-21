#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "httpServer.h"

#include "httpCommandProcessorInterface.h"
#include "httpWorkerRunnables.h"
#include "utils/logger.h"
#include "utils/dataBuffer.h"


namespace server
{


HttpServer::HttpServer(HttpCommandProcessorInterface *processor)
    :
    _command_processor(processor)
{ }

void HttpServer::OnConnect(connection_handler handler)
{
    auto descr = GetConnectionDescriptor(handler);
    if (descr) {
        descr->state = HttpConnectionState::CNeedRequest;
    }
    else {
        throw ConnectionException("HttpServer::OnConnect : invalid handler");
    }
}

void HttpServer::OnRead(connection_handler handler)
{
    auto descr = GetConnectionDescriptor(handler);

    if (descr) {
        if (!descr->_access_lock) {
            // Push task to thread pool
            auto task_type = ReceiveRunnableTypeForConnection(handler);

            if (TestRunnableIsInitialized(task_type)) {
                descr->_access_lock = true;
                ScheduleRunnable(handler, task_type);
            }
            else {
                error("HttpServer::OnRead none tasks for " + std::to_string(handler));
            }
        }
    }
    else {
        error("HttpServer::OnRead bad handler" + std::to_string(handler));
    }

}

void HttpServer::OnWrite(connection_handler handler)
{
    auto descr = GetConnectionDescriptor(handler);

    if (descr) {
        if (!descr->_access_lock) {
            // Push task to thread pool
            auto task_type = ReceiveRunnableTypeForConnection(handler);

            if (TestRunnableIsInitialized(task_type)) {
                descr->_access_lock = true;
                ScheduleRunnable(handler, task_type);
            }
            else {
                error("HttpServer::OnWrite none tasks for " + std::to_string(handler));
            }
        }
    }
    else {
        error("HttpServer::OnWrite bad handler" + std::to_string(handler));
    }
}

void HttpServer::OnDisconnect(connection_handler conn)
{}

void HttpServer::ReadRequest(DataBuffer *buff, HttpRequest& request)
{
    using namespace std;

    ReadDataChunk(buff, request.request_type, ' ');
    debug_string(request.request_type);

    ReadDataChunk(buff, request.path, ' ');
    debug_string(request.path);

    ReadDataChunk(buff, request.protocol);
    debug_string(request.protocol);


    string line;
    ReadDataChunk(buff, line);

    while (!line.empty() && line != "\r")
    {
        debug_string(line);

        ReadDataChunk(buff, line);

        request.other.push_back(line); // TODO: parse for more functionality
    }
}

void HttpServer::ReadDataChunk(
        DataBuffer *in,
        std::string &buffer,
        char delim)
{
    using namespace std;

    buffer.clear();

    for (;;)
    {
        auto next = in->peek_byte();

        if (!next.second ||
            next.first == delim ||
            next.first == '\n')
        {
                break;
        }

        buffer.push_back( in->read_byte().first );
    }

    in->read_byte();  // read delimiter

    if (delim == ' ')
    {
        while (in->peek_byte().first == ' ') in->read_byte();
    }
}


std::unique_ptr<ThreadPool> HttpServer::CreateThreadPool() {
    return std::unique_ptr<ThreadPool>(ThreadPool::ReceiveDefault());
}


HttpWorkerRunnableType HttpServer::ReceiveRunnableTypeForConnection(connection_handler handler) {
    using namespace std;

    auto descr = GetConnectionDescriptor(handler);
        HttpWorkerRunnableType task_type = HttpWorkerRunnableType::ENone;

    if (descr)
    {
    switch (descr->state) {
        case HttpConnectionState::CNone: break;

        case HttpConnectionState::CNeedRequest:
            task_type = HttpWorkerRunnableType::EReadRequest;
            break;

        case HttpConnectionState::CNeedResponse:
            task_type = HttpWorkerRunnableType::EWriteResponse;
            break;

        case HttpConnectionState::CDataSending:
            task_type = HttpWorkerRunnableType::ESendData;
            break;

        case HttpConnectionState::CNeedClose:
            warning("(HttpServer::ReceiveRunnableTypeForConnection) connection closes on main thread");
            break;

        default:
            warning("(HttpServer::ReceiveRunnableTypeForConnection) not implemented state");
            break;
        }
    }
    else {
        throw ConnectionException("HttpServer::ReceiveRunnableTypeForConnection : invalid habdler");
    }

    return task_type;
}


std::unique_ptr<Runnable> HttpServer::CreateRunnableWithType(connection_handler handler, HttpWorkerRunnableType type) {
    using namespace std;

    std::unique_ptr<Runnable> runnable;

    auto descr = GetConnectionDescriptor(handler);

    if (descr) {
        switch (type)
        {
            case HttpWorkerRunnableType::EReadRequest:
                runnable = std::unique_ptr<Runnable>(new HttpServerReadRequestRunnable(handler));
                break;

            case HttpWorkerRunnableType::EWriteResponse:
                runnable = std::unique_ptr<Runnable>(new HttpServerWriteResponseRunnable(handler, _command_processor));
                break;

            case HttpWorkerRunnableType::ESendData:
                runnable = std::unique_ptr<Runnable>(new HttpServerSendDataRunnable());
                break;

            case HttpWorkerRunnableType::ENone:
            default:
                warning("(HttpServer::CreateRunnableWithType) not implemented runnable");
                break;
        }
    }
    else {
        throw ConnectionException("HttpServer::CreateRunnableWithType : invalid habdler");
    }

    return runnable;
}

void HttpServer::ScheduleRunnable(connection_handler handler, const HttpWorkerRunnableType &task_type) {
    std::unique_ptr<Runnable> runnable = CreateRunnableWithType(handler, task_type);
    _runnables.push_back(move(runnable));
}

bool HttpServer::TestRunnableIsInitialized(const HttpWorkerRunnableType &task_type) const
{
    return task_type != HttpWorkerRunnableType::ENone;
}

bool HttpServer::TestConnectionNeedsClose(const connection_descriptor *descr) const
{
    return descr->state == CNeedClose;
}


}

