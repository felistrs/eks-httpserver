#include <assert.h>
#include <utils/logger.h>

#include "ftp_over_http/ftpCommandProcessor.h"

#include "httpThings.h"
#include "utils/fileStorageReader.h"
#include "utils/htmlWrapper.h"
#include "utils/dateTime.h"


namespace server {


FtpCommandProcessor::FtpCommandProcessor(FileStorageReader* file_storage_reader) :
    _file_storage_reader(file_storage_reader)
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
            resp.ChangeStatus(HttpResponse::Status::NotImplemented);
            break;
        }

        // Test path
        bool path_ok = _file_storage_reader->TestIfPathExists(req->path);
        if (path_ok)
        {
            resp.ChangeStatus(HttpResponse::Status::OK);

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

                resp.set_content(content);
            }
            // Is File
            else if (_file_storage_reader->IsFile(req->path))  // Is file
            {
                log("Is file");
                assert(false);
            }
            // Error
            else {
                resp.ChangeStatus(HttpResponse::Status::ServerError);
            }

        }
        else {
            resp.ChangeStatus(HttpResponse::Status::NotFound);
        }

    } while (false);

    // Add "Date:" header
    resp.push_header(HttpResponse::Header::Date, TimeTToString(GMTNow()));

    return resp;
}


}
