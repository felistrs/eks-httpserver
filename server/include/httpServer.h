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
    void OnCommunication(conn_t& conn) override;
    void OnDisconnect(socket_t sock) override;


protected:
    enum conn_state {
        CNone = 0,
        CNeedRecvRequest = 1,
        CNeedSendResponse = 2,
        CNeedClose = 3,
        CIsClosed = 4,
    };


private:
    HttpRequest ReadRequest(std::shared_ptr<Buffer> buff);

    void read_chunk(
            std::shared_ptr<Buffer> in,
            std::string& buff,
            char delim = '\n') const;

};


}

#endif // HTTPSERVER_H
