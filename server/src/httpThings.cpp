#include <map>

#include "httpThings.h"
#include "utils/logger.h"


namespace server {


void HttpResponse::change_status(unsigned status_code)
{
    _responce_status = status_code;

    std::map<unsigned, std::string> status_text = {
        { Status::Continue, "Continue" },
        { Status::OK, "OK" },
        { Status::NotModified, "Not Modified" },
        { Status::BadRequest, "Bad Request" },
        { Status::NotFound, "Not Found" },
        { Status::PreconditionFailed, "Precondition Failed" },
        { Status::ServerError, "Server Error" },
        { Status::NotImplemented, "Not Implemented" }
    };

    auto it = status_text.find(status_code);
    if (it != status_text.end()) {
        _responce_status_text = it->second;
    }
    else {
        _responce_status_text = "Error";
        warning("Unknown status : " + std::to_string(status_code));
    }
}

Buffer server::HttpResponse::Generate()
{
    std::string data =
            _protocol + " " +
            std::to_string(_responce_status) + " " +
            _responce_status_text + "\r\n\0";

    std::vector<char> v;
    std::copy(data.begin(), data.end(), std::back_inserter(v));
    v.push_back(0);
    return Buffer(std::move(v), v.size());
}


}
