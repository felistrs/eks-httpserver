#include "httpServer.h"

#include <cstring>
#include <cctype>

#include <iostream>
#include <sstream>
#include <vector>

#include "utils/logger.h"


namespace srv {


HttpServer::HttpServer()
{}

HttpServer::~HttpServer()
{}

void HttpServer::OnConnect(std::shared_ptr<Socket> sock)
{
    using namespace std;

    istream& in_str = sock->InStream();

    while (true) {
        ReadRequest(in_str);

        if (_comm_processor)
        {
            std::unique_ptr<Command> command(new Command);
            _comm_processor->add(std::move(command));
            _comm_processor->processCommands();
        }

        sock->Close();
        break;  // TODO: !!!
    }
}

HttpRequest HttpServer::ReadRequest(std::istream& in)
{
    using namespace std;

    HttpRequest package;

    read_chunk(in, package.request_type, ' ');
    debug_string(package.request_type);

    read_chunk(in, package.path, ' ');
    debug_string(package.path);

    read_chunk(in, package.protocol);
    debug_string(package.protocol);


    string line;
    read_chunk(in, line);

    while (!line.empty() && line != "\r")
    {
        debug_hex(line);
        debug_string(line);

        read_chunk(in, line);

        package.other.push_back(line); // TODO: parse for more functionality
    }

    return package;
}

void HttpServer::read_chunk(
        std::istream& in,
        std::string& buffer,
        char delim) const
{
    using namespace std;

    buffer.clear();

    while (in.peek() != delim &&
           in.peek() != '\n' &&
           in.peek() != EOF)
    {
        buffer.push_back( (char)in.get() );
    }

    in.get();

    if (delim == ' ')
    {
        while (in.peek() == ' ') in.get();
    }
}


}
