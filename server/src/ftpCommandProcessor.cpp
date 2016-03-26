#include <assert.h>

#include "ftp_over_http/ftpCommandProcessor.h"


namespace server {

FtpCommandProcessor::FtpCommandProcessor(FileStorageReader* fsr) :
    _file_storage_reader(fsr)
{}

HttpResponse FtpCommandProcessor::ProcessRequest(HttpRequest *req)
{
    HttpResponse resp;

    assert(_file_storage_reader);

    return resp;
}


}
