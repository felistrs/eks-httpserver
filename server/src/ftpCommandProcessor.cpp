#include <assert.h>
#include <utils/logger.h>

#include "ftp_over_http/ftpCommandProcessor.h"
#include "utils/htmlWrapper.h"
#include "utils/dateTime.h"


namespace server {

FtpCommandProcessor::FtpCommandProcessor(FileStorageReader* fsr) :
    _file_storage_reader(fsr)
{}

HttpResponse FtpCommandProcessor::ProcessRequest(HttpRequest *req)
{
    assert(_file_storage_reader);
    assert(req);

    HttpResponse resp;

    do {
        // Method
        if (req->request_type != "GET") // Process only GET
        {
            resp.change_status(HttpResponse::Status::NotImplemented);
            break;
        }

        // Test path
        bool path_ok = _file_storage_reader->TestIfPathExists(req->path);
        if (path_ok)
        {
            resp.change_status(HttpResponse::Status::OK);

            // Is Folder
            if (_file_storage_reader->IsFolder(req->path))
            {
                auto files = _file_storage_reader->GetFilesList(req->path);

                HtmlWrapper html;
                html.begin_paragraph();
                html.push_header("Folder : " + req->path);
                html.end_paragraph();
                html.begin_list();
                for (const auto& file : files)
                {
                    html.push_list_item(file);
                }
                html.end_list();

                std::string content = html.GenerateHtml();

                resp.push_header(HttpResponse::Header::ContentType, "text/html");
                resp.push_header(HttpResponse::Header::ContentLength, std::to_string(content.size()) );

                resp.push_header(HttpResponse::Header::Date, TimeTToString(GMTNow()));

                resp.set_content(content);
            }
            // Is File
            else if (_file_storage_reader->IsFile(req->path))  // Is file
            {
                log("Is file");
            }
            // Error
            else {
                resp.change_status(HttpResponse::Status::ServerError);
            }

        }
        else {
            resp.change_status(HttpResponse::Status::NotFound);
        }

    } while (false);

    return resp;
}


}
