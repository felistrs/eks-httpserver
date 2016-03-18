#ifndef HTTP_COMMAND_PROCESSOR_INTERFACE_H
#define HTTP_COMMAND_PROCESSOR_INTERFACE_H

#include <memory>

#include "httpThings.h"


namespace srv {


class HttpCommandProcessorInterface
{
public:
    HttpCommandProcessorInterface() {}
    virtual ~HttpCommandProcessorInterface() {}

    virtual std::shared_ptr<HttpResponce> ProcessRequest(HttpRequest* req) = 0;
};


}

#endif // HTTP_COMMAND_PROCESSOR_INTERFACE_H
