#include <cassert>
#include <cctype>
#include <cstring>

#include <iostream>
#include <sstream>
#include <vector>

#include "httpServer.h"

#include "utils/logger.h"
#include "socket/socket.h"


namespace server {


HttpServer::HttpServer(HttpCommandProcessorInterface *processor) :
    _command_processor(processor)
{}

void HttpServer::OnConnect(connection_handler handler)
{
    auto descr = GetConnectionDescriptor(handler);
    if (descr)
    {
        descr->state = conn_state::CNeedReqResp;
    }
    else {
        throw ConnectionException("HttpServer::OnConnect : invalid handler");
    }
}

void HttpServer::OnCommunication(connection_handler handler)
{
    using namespace std;

    auto descr = GetConnectionDescriptor(handler);

    if (descr)
    {
        switch (descr->state) {
        case conn_state::CNone:
            warning("(HttpServer::DoCommunication) : " +
                    to_string(descr->sock_handler) +
                    " connection not initialized.");
            break;

        case conn_state::CNeedReqResp:
        {
            // Read request
            DataBuffer buff = GetBuffer(handler);
            HttpRequest request = ReadRequest(&buff);

            //
            assert(_command_processor);
            HttpResponse response = _command_processor->ProcessRequest(&request);

            // Send responce
            auto buffer = response.Generate();
            sock::SendBuffer(handler, &buffer);

            if (response.DoCloseConnection()) {
                descr->state = conn_state::CNeedClose;
            }
            else {
                descr->state = conn_state::CDataSending;
            }

            break;
        }

        case conn_state::CDataSending:
            assert(false);
            break;

        case conn_state::CNeedClose:
            CloseConnection(handler);
            break;

        default:
            warning("(HttpServer::DoCommunication) not implemented state");
            break;
        }
    }
    else {
        throw ConnectionException("HttpServer::DoCommunication : invalid habdler");
    }
}

void HttpServer::OnDisconnect(connection_handler conn)
{}

DataBuffer HttpServer::GetBuffer(connection_handler conn)
{
    auto buff = sock::RecvBuffer(conn);

    std::cout << "Read buffer sz: " << buff.data().size() << std::endl;
//    debug_hex(buff.data());
    debug_string(buff.data());
    std::cout << std::endl;

    return buff;
}

HttpRequest HttpServer::ReadRequest(DataBuffer* buff)
{
    using namespace std;

    HttpRequest package;

    read_chunk(buff, package.request_type, ' ');
    debug_string(package.request_type);

    read_chunk(buff, package.path, ' ');
    debug_string(package.path);

    read_chunk(buff, package.protocol);
    debug_string(package.protocol);


    string line;
    read_chunk(buff, line);

    while (!line.empty() && line != "\r")
    {
//        debug_hex(line);
        debug_string(line);

        read_chunk(buff, line);

        package.other.push_back(line); // TODO: parse for more functionality
    }

    return package;
}

void HttpServer::read_chunk(
        DataBuffer* in,
        std::string& buffer,
        char delim) const
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

bool HttpServerTaskRunner(ThreadTask* task)
{
    log("HttpServerTaskRunner task : " + std::to_string(task->id) );
    return true;
}

}

std::unique_ptr<ThreadPool> HttpServer::CreateThreadPool() {
    return std::unique_ptr<ThreadPool>(new ThreadPool(HttpServerTaskRunner));
}


}
