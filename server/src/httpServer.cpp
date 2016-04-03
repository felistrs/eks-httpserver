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
            warning("(HttpServer::OnCommunication) : " +
                    to_string(descr->sock_handler) +
                    " connection not initialized.");
            break;

        case conn_state::CNeedReqResp:
        {
            // Read request
            Buffer buff = GetBuffer(handler);
            HttpRequest request = ReadRequest(&buff);

            //
            assert(_command_processor);
            HttpResponse response = _command_processor->ProcessRequest(&request);

            // Send responce
            auto buffer = response.Generate();
            sock::SendBuffer(handler, &buffer);

            break;
        }

        case conn_state::CDataSending:
            break;

        case conn_state::CNeedClose:
            break;

        default:
            warning("(HttpServer::OnCommunication) not implemented state");
            break;
        }
    }
    else {
        throw ConnectionException("HttpServer::OnCommunication : invalid habdler");
    }
}

void HttpServer::OnDisconnect(connection_handler conn)
{}

Buffer HttpServer::GetBuffer(connection_handler conn)
{
    auto buff = sock::RecvBuffer(conn);

    std::cout << "Read buffer sz: " << buff.data().size() << std::endl;
    debug_hex(buff.data());
    debug_string(buff.data());
    std::cout << std::endl;

    return buff;
}

HttpRequest HttpServer::ReadRequest(Buffer* buff)
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
        debug_hex(line);
        debug_string(line);

        read_chunk(buff, line);

        package.other.push_back(line); // TODO: parse for more functionality
    }

    return package;
}

void HttpServer::read_chunk(
        Buffer* in,
        std::string& buffer,
        char delim) const
{
    using namespace std;

    buffer.clear();

    while (in->peek() != delim &&
           in->peek() != '\n' &&
           in->peek() != EOF)
    {
        buffer.push_back( in->get() );
    }

    in->get();

    if (delim == ' ')
    {
        while (in->peek() == ' ') in->get();
    }
}


}
