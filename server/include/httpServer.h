#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "server.h"

#include <memory>
#include <string>
#include <utility>

#include "httpCommandProcessorInterface.h"
#include "httpThings.h"


namespace server {


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void OnConnect(connection_descriptor& conn) override;
    void OnCommunication(connection_descriptor& conn) override;
    void OnDisconnect(connection_descriptor& conn) override;

    void setup_command_processor(HttpCommandProcessorInterface* processor);


protected:
    enum conn_state {
        CNone = 0,
        CNeedReqResp,
        CDataSending,
        CNeedClose,
    };


private:
    std::shared_ptr<Buffer> GetBuffer(connection_descriptor& conn);

    HttpRequest ReadRequest(std::shared_ptr<Buffer> buff);

    void read_chunk(
            std::shared_ptr<Buffer> in,
            std::string& buff,
            char delim = '\n') const;

    std::unique_ptr<HttpCommandProcessorInterface> _comm_processor;
};


}

#endif // HTTP_SERVER_H
