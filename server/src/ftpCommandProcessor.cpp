#include <assert.h>

#include "ftp_over_http/ftpCommandProcessor.h"
#include "utils/htmlWrapper.h"


namespace server {

FtpCommandProcessor::FtpCommandProcessor(FileStorageReader* fsr) :
    _file_storage_reader(fsr)
{}

HttpResponse FtpCommandProcessor::ProcessRequest(HttpRequest *req)
{
    assert(_file_storage_reader);

    HttpResponse resp;

    bool path_ok = _file_storage_reader->TestIfPathExists(req->path);
    if (path_ok)
    {
        resp.change_status(HttpResponse::Status::OK);

//        if (_file_storage_reader->IsFolder(req->path))  // Is folder
//        {
////            resp.set_text(...);
//        }
//        else {  // Is file

//        }
    }
    else {
        resp.change_status(HttpResponse::Status::NotFound);
    }

    return resp;
}


}
