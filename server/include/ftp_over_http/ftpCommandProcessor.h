#ifndef FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H

#include <memory>

#include "httpCommandProcessorInterface.h"
#include "utils/fileStorageReader.h"


namespace srv {


class FtpCommandProcessor : public HttpCommandProcessorInterface
{
public:
    FtpCommandProcessor(FileStorageReader* fsr);
    virtual ~FtpCommandProcessor() {}

    std::shared_ptr<HttpResponce> ProcessRequest(HttpRequest* req) override;

private:
    std::unique_ptr<FileStorageReader> _file_storage_reader;
};


}

#endif // FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
