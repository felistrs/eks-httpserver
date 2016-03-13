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
{
    using namespace std;

    // TMP
    const ssize_t c_read_buffer_sz = 2048; // ???
    std::stringbuf _sbuffer;
    std::vector<char> _read_buffer(c_read_buffer_sz);

    ssize_t read_sz = recv(sock, _read_buffer.data(),
                           c_read_buffer_sz, 0);
    std::cout << "Read buffer sz: " << read_sz << std::endl;
    debug_hex(_read_buffer);
    debug_string(_read_buffer);
    std::cout << std::endl;

    _sbuffer.sputn(_read_buffer.data(), read_sz);

    istream in_str(&_sbuffer);
    // END: TMP


    while (true)
    {
        HttpRequest request = ReadRequest(in_str);

        if (_comm_processor)
        {
            std::unique_ptr<Command> command(
                        new FtpCommand(request));
            _comm_processor->add(std::move(command));
            _comm_processor->processCommands();
        }

        Socket::Close(sock);
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
