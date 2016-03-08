#include "httpServer.h"

#include <cstring>

#include <iostream>
#include <sstream>
#include <vector>


namespace srv {


HttpServer::HttpServer()
{
}

HttpServer::~HttpServer()
{
}

void HttpServer::OnConnect(int conn_fd)
{
    using namespace std;

    ssize_t buffer_sz = 1024;
    vector<buff_t> buffer(buffer_sz);

    while (true) {
        // TODO: move to socket wrapper
        ssize_t read_sz = recv(conn_fd, buffer.data(), buffer_sz, 0);
        cout << "Read buffer sz: " << read_sz << endl;

        stringbuf sbuffer;
        sbuffer.sputn(buffer.data(), read_sz);

        istream in(&sbuffer);


        ReadRequest(in);

        break; // TODO: !!!
    }
}

void HttpServer::ReadRequest(std::istream& in)
{
    using namespace std;

    vector<buff_t> request_type;
    read_chunk(in, request_type, ' ');
    cout << "Read sz: " << request_type.size() << endl;
    debug_hex(request_type);

    vector<buff_t> request_path;
    read_chunk(in, request_path, ' ');
    cout << "Read sz: " << request_path.size() << endl;
    debug_hex(request_path);

    vector<buff_t> request_protocol;
    read_chunk(in, request_protocol);
    cout << "Read sz: " << request_protocol.size() << endl;
    debug_hex(request_protocol);

//  ...
}

void HttpServer::read_chunk(
        std::istream& in,
        std::vector<HttpServer::buff_t>& buffer,
        HttpServer::buff_t delim) const
{
    using namespace std;

    buffer.clear();

    while (in.peek() != delim &&
           in.peek() != '\n' &&
           in.peek() != EOF)
    {
        buffer.push_back( (buff_t)in.get() );
    }

    in.get();

    if (delim == ' ')
    {
        while (in.peek() == ' ') in.get();
    }
}

void HttpServer::debug_hex(const std::vector<HttpServer::buff_t> &buffer) const
{
    using namespace std;

    cout << hex;

    for (auto c : buffer) {
        cout << unsigned(c) << " ";
    }
    cout << dec << endl << endl;
}


}
