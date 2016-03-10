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

    void OnConnect(std::shared_ptr<Socket> sock) override;

private:
    struct package_in {
        std::vector<Socket::buff_t> request_type;
        std::vector<Socket::buff_t> request_path;
        std::vector<Socket::buff_t> request_protocol;
    };


    package_in ReadRequest(std::istream& in);

    void read_chunk(
            std::istream& in,
            std::vector<Socket::buff_t>& buffer,
            Socket::buff_t delim = '\n') const;

    void debug_hex(const std::vector<Socket::buff_t>& buffer) const;
    void debug_string(const std::vector<Socket::buff_t>& buffer) const;
};


}

#endif // HTTPSERVER_H
