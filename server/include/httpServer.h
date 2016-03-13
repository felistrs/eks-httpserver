#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server.h"

#include <memory>
#include <string>
#include <utility>


namespace srv {


struct HttpRequest {
    std::string request_type;
    std::string path;
    std::string protocol;

    std::vector<std::string> other;
};


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void OnConnect(socket_t sock) override;

private:
    HttpRequest ReadRequest(std::istream& in);

    void read_chunk(
            std::istream& in,
            std::string& buffer,
            char delim = '\n') const;

};


}

#endif // HTTPSERVER_H
