#include "ftp_over_http/ftpCommandProcessor.h"

namespace server {

FtpCommandProcessor::FtpCommandProcessor(FileStorageReader* fsr) :
    _file_storage_reader(fsr)
{}

std::shared_ptr<HttpResponse> FtpCommandProcessor::ProcessRequest(HttpRequest *req)
{
    std::shared_ptr<HttpResponse> resp(new HttpResponse);

    return resp;
}


}
