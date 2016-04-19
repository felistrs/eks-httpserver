#include <assert.h>
#include "../utils/logger.h"

#include "ftpCommandProcessor.h"

#include "../httpThings.h"
#include "../utils/fileStorageReader.h"
#include "../utils/htmlWrapper.h"
#include "../utils/dateTime.h"


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
        if (TestUnimplementedMethods(req)) // Process only GET
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

                GenerateResponseWithFolderContent(req, resp, files);
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

void FtpCommandProcessor::GenerateResponseWithFolderContent(
        const HttpRequest *req, HttpResponse &resp,
        const std::vector<std::string> &files) const
{
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

    std::__cxx11::string content = html.GenerateHtml();

    resp.push_header(HttpResponse::ContentType, "text/html");
    resp.push_header(HttpResponse::ContentLength, std::__cxx11::to_string(content.size()) );

    resp.set_content(content);
}

bool FtpCommandProcessor::TestUnimplementedMethods(const HttpRequest *req) const
{
    return req->request_type != "GET";
}


}
