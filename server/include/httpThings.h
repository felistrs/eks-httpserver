#ifndef HTTP_THINGS_H
#define HTTP_THINGS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "utils/dataBuffer.h"


namespace server {


struct HttpRequest {
    std::string request_type;
    std::string path;
    std::string protocol;

    std::vector<std::string> other;
};


class HttpResponse {
public:
    enum Status {
        Continue = 100,
        OK = 200,
        NotModified = 304,
        BadRequest = 400,
        NotFound = 404,
        PreconditionFailed = 412,
        ServerError = 500,
        NotImplemented = 501
    };

    enum Header {
        Date = 0,
        ContentType,
        ContentLength
    };

    void change_status(unsigned status_code);
    void push_header(Header header, std::string text);

    DataBuffer Generate();

    bool DoCloseConnection() { return _close_connection_after_first_response; }

public:
    void set_content(std::string text) { _content = text; }


private:
    const std::string c_new_line = "\n";

    const std::map<unsigned, std::string> c_status_string = {
            { Status::Continue, "Continue" },
            { Status::OK, "OK" },
            { Status::NotModified, "Not Modified" },
            { Status::BadRequest, "Bad Request" },
            { Status::NotFound, "Not Found" },
            { Status::PreconditionFailed, "Precondition Failed" },
            { Status::ServerError, "Server Error" },
            { Status::NotImplemented, "Not Implemented" }
    };

    const std::map<unsigned, std::string> c_header_string = {
            { Header::Date, "Date" },
            { Header::ContentType, "Content-Type" },
            { Header::ContentLength, "Content-Length" }
    };


    std::string _protocol = "HTTP/1.1";
    unsigned _response_status = 200;
    std::string _response_status_text = "OK";

    std::map<std::string, std::string> _headers;
    std::string _content;

    bool _close_connection_after_first_response = true;  // TODO: change for file sending
};


}


#endif // HTTP_THINGS_H
