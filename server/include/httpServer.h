#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server.h"

#include <memory>
#include <string>
#include <utility>

#include "httpCommandProcessorInterface.h"
#include "httpThings.h"


namespace srv {


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void OnConnect(conn_t& conn) override;
    void OnCommunication(conn_t& conn) override;
    void OnDisconnect(conn_t& conn) override;

    void set_command_processor(HttpCommandProcessorInterface* processor);


protected:
    enum conn_state {
        CNone = 0,
        CNeedReqResp,
        CDataSending,
        CNeedClose,
    };


private:
    std::shared_ptr<Buffer> GetBuffer(conn_t& conn);

    HttpRequest ReadRequest(std::shared_ptr<Buffer> buff);

    void read_chunk(
            std::shared_ptr<Buffer> in,
            std::string& buff,
            char delim = '\n') const;

    std::unique_ptr<HttpCommandProcessorInterface> _comm_processor;
};


}

#endif // HTTPSERVER_H
