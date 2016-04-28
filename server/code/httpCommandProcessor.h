#ifndef FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H

#include <vector>
#include "httpCommandProcessorInterface.h"


class FileStorageReader;


namespace server {


class HttpRequest;
class HttpResponse;


class HttpCommandProcessor : public HttpCommandProcessorInterface
{
public:
    HttpCommandProcessor(FileStorageReader* file_storage_reader);
    virtual ~HttpCommandProcessor() {}

    HttpResponse ProcessRequest(HttpRequest* req) override;

private:
    bool TestUnimplementedMethods(const HttpRequest *req) const;
    void GenerateResponseWithFolderContent(
            const HttpRequest *req, HttpResponse &resp,
            const std::vector<std::string> &files) const;


    FileStorageReader *_file_storage_reader;

};


}

#endif // FTP_OVER_HTTP__FTP_COMMAND_PROCESSOR_H
