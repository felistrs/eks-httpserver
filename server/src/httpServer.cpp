#include "httpServer.h"

#include <cstring>
#include <cctype>

#include <iostream>
#include <sstream>
#include <vector>


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

        break; // TODO: !!!
    }
}

HttpServer::package_in HttpServer::ReadRequest(std::istream& in)
{
    using namespace std;

    package_in package;

    read_chunk(in, package.request_type, ' ');
    cout << "Read sz: " << package.request_type.size() << endl;
    debug_hex(package.request_type);
    debug_string(package.request_type);

    read_chunk(in, package.request_path, ' ');
    cout << "Read sz: " << package.request_path.size() << endl;
    debug_hex(package.request_path);
    debug_string(package.request_path);

    read_chunk(in, package.request_protocol);
    cout << "Read sz: " << package.request_protocol.size() << endl;
    debug_hex(package.request_protocol);
    debug_string(package.request_protocol);


//    vector<buff_t> line;
//    read_chunk(in, line);

//    while (line.size() && !(line.size() == 1 && line[0] == '\r'))
//    {
//        debug_hex(line);
//        debug_string(line);

//        read_chunk(in, line);
//    }

//  ...

    return package;
}

void HttpServer::read_chunk(
        std::istream& in,
        std::vector<Socket::buff_t>& buffer,
        Socket::buff_t delim) const
{
    using namespace std;

    buffer.clear();

    while (in.peek() != delim &&
           in.peek() != '\n' &&
           in.peek() != EOF)
    {
        buffer.push_back( (Socket::buff_t)in.get() );
    }

    in.get();

    if (delim == ' ')
    {
        while (in.peek() == ' ') in.get();
    }
}

void HttpServer::debug_hex(const std::vector<Socket::buff_t> &buffer) const
{
    using namespace std;

    cout << hex;

    for (auto c : buffer) {
        cout << unsigned(c) << " ";
    }
    cout << dec << endl << endl;
}

void HttpServer::debug_string(const std::vector<Socket::buff_t> &buffer) const
{
    using namespace std;

    std::string str;
    for (Socket::buff_t c : buffer)
        if (isprint(c)) str += c;

    cout << str << endl << endl;
}


}
