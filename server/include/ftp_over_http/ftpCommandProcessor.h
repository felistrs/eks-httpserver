#ifndef FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H

#include "httpCommandProcessorInterface.h"


namespace srv {


class FtpCommandProcessor : public HttpCommandProcessorInterface
{
public:
    FtpCommandProcessor() {}
    virtual ~FtpCommandProcessor() {}

    std::shared_ptr<HttpResponce> ProcessRequest(HttpRequest* req) override;
};


}

#endif // FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
