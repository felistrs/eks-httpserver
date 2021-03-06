#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <list>
#include <memory>
#include <string>
#include <utility>

#include "server.h"
#include "httpThings.h"
#include "httpWorkerRunnables.h"


class DataBuffer;

namespace server {


class HttpCommandProcessorInterface;
struct HttpThreadTask;


class HttpServer : public Server
{
public:
    HttpServer(HttpCommandProcessorInterface* processor);
    virtual ~HttpServer() {}

    static void ReadRequest(DataBuffer* buff, HttpRequest& request_out);

    static void ReadDataChunk(
            DataBuffer *in,
            std::string &buff,
            char delim = '\n');

protected:
    void OnConnect(connection_handler conn) override;
    void OnRead(connection_handler conn) override;
    void OnWrite(connection_handler conn) override;
    void OnDisconnect(connection_handler conn) override;

    virtual std::unique_ptr<ThreadPool> CreateThreadPool();

    HttpWorkerRunnableType ReceiveRunnableTypeForConnection(connection_handler handler);
    std::unique_ptr<ServerRunnable> CreateRunnableWithType(connection_handler handler, HttpWorkerRunnableType type);

private:
    virtual bool TestConnectionNeedsClose(const connection_descriptor *descr) const;
    bool TestRunnableIsInitialized(const HttpWorkerRunnableType &task_type) const;
    void ScheduleRunnable(connection_handler handler, const HttpWorkerRunnableType &task_type);


    HttpCommandProcessorInterface* _command_processor;

};


}

#endif // HTTP_SERVER_H
