#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <list>
#include <memory>
#include <string>
#include <utility>

#include "server.h"
#include "httpThings.h"


namespace server {


class HttpCommandProcessorInterface;
struct HttpThreadTask;


class HttpServer : public Server
{
public:
    HttpServer(HttpCommandProcessorInterface* processor);
    virtual ~HttpServer() {}

    static HttpRequest ReadRequest(DataBuffer* buff);

    static void ReadDataChunk(
            DataBuffer *in,
            std::string &buff,
            char delim = '\n');

protected:
    void OnConnect(connection_handler conn) override;
    void OnCommunication(connection_handler conn) override;
    void OnDisconnect(connection_handler conn) override;

    virtual std::unique_ptr<ThreadPool> CreateThreadPool();

    HttpThreadTask::Type ReceiveTaskTypeForConnection(connection_handler handler);

private:
    long _thread_task_counter = 0;

    HttpCommandProcessorInterface* _command_processor;

};


}

#endif // HTTP_SERVER_H
