#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server.h"

#include <utility>


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void on_connect(int conn_fd);

private:
    using buff_t = char;

    void read_request(std::istream& in);
    void read_limited(std::istream& in, std::vector<buff_t> buffer, buff_t delim = '\n');

    void debug_hex(const std::vector<buff_t>& buffer);
};

#endif // HTTPSERVER_H
