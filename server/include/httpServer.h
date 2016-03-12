#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server.h"

#include <utility>
#include <string>


namespace srv {


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void OnConnect(std::shared_ptr<Socket> sock) override;

private:
    struct package_in {
        std::string request_type;
        std::string path;
        std::string protocol;

        std::vector<std::string> other;
    };


    package_in ReadRequest(std::istream& in);

    void read_chunk(
            std::istream& in,
            std::string& buffer,
            char delim = '\n') const;

};


}

#endif // HTTPSERVER_H
