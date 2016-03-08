#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server.h"

#include <utility>


namespace srv {


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void OnConnect(int conn_fd);

private:
    using buff_t = char;


    void ReadRequest(std::istream& in);

    void read_chunk(
            std::istream& in,
            std::vector<buff_t>& buffer,
            buff_t delim = '\n') const;

    void debug_hex(const std::vector<buff_t>& buffer) const;
};


}

#endif // HTTPSERVER_H
