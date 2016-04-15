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
            auto task_type = ReceiveRunnableTypeForConnection(handler);

            if (task_type != HttpWorkerRunnableType::ENone) {
                std::unique_ptr<Runnable> runnable = CreateRunnableWithType(handler, task_type);
                _http_runners.push_back(std::move(runnable));
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

HttpRequest HttpServer::ReadRequest(DataBuffer *buff)
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

//bool HttpServerTaskRunner(ThreadTask* task)
//{
//    bool task_is_done = false;
//
//    log("HttpServerTaskRunner task : " + std::to_string(task->id) );
//
//    HttpThreadTask* task_ = dynamic_cast<HttpThreadTask*>(task);
//    if (task_ == nullptr)
//    {
//        log("HttpServerTaskRunner task : " + std::to_string(task->id) + " type is bad" );
//    }
//    else {
//        switch (task_->task) {
//            case HttpThreadTask::Type::ENone:
//                task_is_done = true;
//                break;
//
//            case HttpThreadTask::Type::EDoResponse:
//                task_is_done = HttpServerTaskDoResponse(task_);
//                break;
//
//            case HttpThreadTask::Type::ESendData:
//                task_is_done = HttpServerTaskSendData(task_);
//                break;
//
//            default:
//                error("HttpServerTaskRunner: unemplimaented task");
//                break;
//        }
//    }
//
//    return task_is_done;
//}

}


std::unique_ptr<ThreadPool> HttpServer::CreateThreadPool() {
    return std::unique_ptr<ThreadPool>(new ThreadPool()); //HttpServerTaskRunner));
}

    HttpWorkerRunnableType HttpServer::ReceiveRunnableTypeForConnection(connection_handler handler) {
    using namespace std;

    auto descr = GetConnectionDescriptor(handler);
        HttpWorkerRunnableType task_type = HttpWorkerRunnableType::ENone;

    if (descr)
    {
        switch (descr->state) {
            case HttpConnectionState::CNone: break;

            case HttpConnectionState::CNeedReqResp:
                task_type = HttpWorkerRunnableType::EDoResponse;
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
            case HttpWorkerRunnableType::EDoResponse:
                runnable = std::unique_ptr<Runnable>(new HttpServerDoResponseRunnable(handler, _command_processor));
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


}
