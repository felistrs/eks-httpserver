#include "ftp_over_http/ftpCommandProcessor.h"

namespace srv {

FtpCommandProcessor::FtpCommandProcessor(FileStorageReader* fsr) :
    _file_storage_reader(fsr)
{}

std::shared_ptr<HttpResponce> FtpCommandProcessor::ProcessRequest(HttpRequest *req)
{
    std::shared_ptr<HttpResponce> resp(new HttpResponce);

    return resp;
}


}
