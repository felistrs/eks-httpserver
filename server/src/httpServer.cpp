#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "httpServer.h"

#include "httpCommandProcessorInterface.h"
#include "utils/dataBuffer.h"
#include "utils/logger.h"


namespace server {


HttpServer::HttpServer(HttpCommandProcessorInterface *processor) :
    _command_processor(processor)
{}

void HttpServer::OnConnect(connection_handler handler)
{
    auto descr = GetConnectionDescriptor(handler);
    if (descr)
    {
        descr->state = HttpConnectionState::CNeedReqResp;
    }
    else {
        throw ConnectionException("HttpServer::OnConnect : invalid handler");
    }
}

void HttpServer::OnCommunication(connection_handler handler)
{
    auto descr = GetConnectionDescriptor(handler);

    if (descr)
    {
        // Close connection
        if (descr->state == HttpConnectionState::CNeedClose) {
            CloseConnection(handler);
        }
        else {
            // Push task to thread pool
            auto task_type = ReceiveTaskTypeForConnection(handler);

            if (task_type != HttpThreadTask::Type::ENone) {
                std::unique_ptr<HttpThreadTask> task(new HttpThreadTask);
                task->id = _thread_task_counter++;
                task->task = task_type;
                task->connection = handler;
                task->command_processor = _command_processor;
                _http_tasks.push_back(std::move(task));
            }
            else {
                error("HttpServer::OnCommunication none tasks for " + std::to_string(handler));
            }
        }
    }
    else {
        error("HttpServer::OnCommunication bad handler" + std::to_string(handler));
    }

}

void HttpServer::OnDisconnect(connection_handler conn)
{}

HttpRequest HttpServer::ReadRequest(DataBuffer* buff)
{
    using namespace std;

    HttpRequest package;

    ReadDataChunk(buff, package.request_type, ' ');
    debug_string(package.request_type);

    ReadDataChunk(buff, package.path, ' ');
    debug_string(package.path);

    ReadDataChunk(buff, package.protocol);
    debug_string(package.protocol);


    string line;
    ReadDataChunk(buff, line);

    while (!line.empty() && line != "\r")
    {
        debug_string(line);

        ReadDataChunk(buff, line);

        package.other.push_back(line); // TODO: parse for more functionality
    }

    return package;
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


namespace {

bool HttpServerTaskDoResponse(HttpThreadTask* task)
{
    bool task_is_done = false;

    // Read request
    DataBuffer buff = Server::ReadBuffer(task->connection);
    HttpRequest request = HttpServer::ReadRequest(&buff);

    //
    assert(task->command_processor);
    HttpResponse response = task->command_processor->ProcessRequest(&request);

    // Send responce
    auto buffer = response.Generate();
    sock::SendBuffer(task->connection, &buffer);

    // change stat
    auto descr = GetConnectionDescriptor(task->connection);

    if (response.DoCloseConnection()) {
        descr->state = HttpConnectionState::CNeedClose;
    }
    else {
        descr->state = HttpConnectionState::CDataSending;
    }

    task->completed = true;
    return task_is_done;
}

bool HttpServerTaskSendData(HttpThreadTask* task)
{
    bool task_is_done = false;

    assert(false); // TODO: this

    task->completed = true;

    return task_is_done;
}

bool HttpServerTaskRunner(ThreadTask* task)
{
    bool task_is_done = false;

    log("HttpServerTaskRunner task : " + std::to_string(task->id) );

    HttpThreadTask* task_ = dynamic_cast<HttpThreadTask*>(task);
    if (task_ == nullptr)
    {
        log("HttpServerTaskRunner task : " + std::to_string(task->id) + " type is bad" );
    }
    else {
        switch (task_->task) {
            case HttpThreadTask::Type::ENone:
                task_is_done = true;
                break;

            case HttpThreadTask::Type::EDoResponse:
                task_is_done = HttpServerTaskDoResponse(task_);
                break;

            case HttpThreadTask::Type::ESendData:
                task_is_done = HttpServerTaskSendData(task_);
                break;

            default:
                error("HttpServerTaskRunner: unemplimaented task");
                break;
        }
    }

    return task_is_done;
}

}


std::unique_ptr<ThreadPool> HttpServer::CreateThreadPool() {
    return std::unique_ptr<ThreadPool>(new ThreadPool(HttpServerTaskRunner));
}

HttpThreadTask::Type HttpServer::ReceiveTaskTypeForConnection(connection_handler handler) {
    using namespace std;

    auto descr = GetConnectionDescriptor(handler);
    HttpThreadTask::Type task_type = HttpThreadTask::Type::ENone;

    if (descr)
    {
        switch (descr->state) {
            case HttpConnectionState::CNone: break;

            case HttpConnectionState::CNeedReqResp:
                task_type = HttpThreadTask::Type::EDoResponse;
                break;

            case HttpConnectionState::CDataSending:
                task_type = HttpThreadTask::Type::ESendData;
                break;

            case HttpConnectionState::CNeedClose:
                warning("(HttpServer::ReceiveTaskTypeForConnection) connection closes on main thread");
                break;

            default:
                warning("(HttpServer::ReceiveTaskTypeForConnection) not implemented state");
                break;
        }
    }
    else {
        throw ConnectionException("HttpServer::ReceiveTaskTypeForConnection : invalid habdler");
    }

    return task_type;
}


}
