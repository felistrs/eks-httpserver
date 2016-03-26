#ifndef FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H

#include <memory>

#include "httpCommandProcessorInterface.h"
#include "utils/fileStorageReader.h"


namespace server {


class FtpCommandProcessor : public HttpCommandProcessorInterface
{
public:
    FtpCommandProcessor(FileStorageReader* fsr);
    virtual ~FtpCommandProcessor() {}

    HttpResponse ProcessRequest(HttpRequest* req) override;

private:
    FileStorageReader* _file_storage_reader;
};


}

#endif // FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H
