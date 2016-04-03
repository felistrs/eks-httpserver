#ifndef HTTP_THINGS_H
#define HTTP_THINGS_H

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

    void change_status(unsigned status_code);

    DataBuffer Generate();

public:
    void set_text(std::string text) { _text = text; }


private:
    std::string _protocol = "HTTP/1.1";
    unsigned _responce_status = 200;
    std::string _responce_status_text = "OK";

    std::string _text;
};


}


#endif // HTTP_THINGS_H
