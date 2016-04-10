#ifndef FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H

#include "httpCommandProcessorInterface.h"


class FileStorageReader;


namespace server {


class HttpRequest;
class HttpResponse;


class FtpCommandProcessor : public HttpCommandProcessorInterface
{
public:
    FtpCommandProcessor(FileStorageReader* file_storage_reader);
    virtual ~FtpCommandProcessor() {}

    HttpResponse ProcessRequest(HttpRequest* req) override;

private:
    FileStorageReader *_file_storage_reader;
};


}

#endif // FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H
