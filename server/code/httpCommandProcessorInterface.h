#ifndef HTTP_COMMAND_PROCESSOR_INTERFACE_H
#define HTTP_COMMAND_PROCESSOR_INTERFACE_H

#include <memory>


namespace server {


class HttpResponse;
class HttpRequest;


class HttpCommandProcessorInterface
{
public:
    HttpCommandProcessorInterface() {}
    virtual ~HttpCommandProcessorInterface() {}

    virtual HttpResponse ProcessRequest(HttpRequest* req) = 0;
};


}

#endif // HTTP_COMMAND_PROCESSOR_INTERFACE_H
