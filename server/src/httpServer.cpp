#include "httpServer.h"

#include <cstring>
#include <cctype>

#include <iostream>
#include <sstream>
#include <vector>

#include "utils/logger.h"
#include "ftp_over_http/ftpCommand.h" // TODO: remove !


namespace srv {


HttpServer::HttpServer()
{}

HttpServer::~HttpServer()
{}

void HttpServer::OnConnect(socket_t sock)
{}

void HttpServer::OnCommunication(Server::conn_t &conn)
{
    using namespace std;

    shared_ptr<Buffer> buff = ReadBuffer(conn.sock);

    if (!buff)
    {
        warning("Buffer size id zero !");
    } else {
        std::cout << "Read buffer sz: " << buff->data().size() << std::endl;

        debug_hex(buff->data());
        debug_string(buff->data());
        std::cout << std::endl;

        while (true)
        {
            HttpRequest request = ReadRequest(buff);

            if (_comm_processor)
            {
                std::unique_ptr<Command> command(
                            new FtpCommand(request));
                _comm_processor->add(std::move(command));
                _comm_processor->processCommands();
            }

            break;  // TODO: !!!
        }
    }
}

void HttpServer::OnDisconnect(socket_t sock)
{}

HttpRequest HttpServer::ReadRequest(std::shared_ptr<Buffer> buff)
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
        std::shared_ptr<Buffer> in,
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
