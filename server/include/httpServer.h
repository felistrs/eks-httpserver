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
    HttpServer(HttpCommandProcessorInterface* processor);
    virtual ~HttpServer() {}

    void OnConnect(connection_handler conn) override;
    void OnCommunication(connection_handler conn) override;
    void OnDisconnect(connection_handler conn) override;

protected:
    enum conn_state {
        CNone = 0,
        CNeedReqResp,
        CDataSending,
        CNeedClose,
    };

    virtual std::unique_ptr<ThreadPool> CreateThreadPool();

private:
    DataBuffer GetBuffer(connection_handler conn);

    HttpRequest ReadRequest(DataBuffer* buff);

    void read_chunk(
            DataBuffer* in,
            std::string& buff,
            char delim = '\n') const;


    HttpCommandProcessorInterface* _command_processor;
};


}

#endif // HTTP_SERVER_H
