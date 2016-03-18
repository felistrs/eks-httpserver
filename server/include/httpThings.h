#ifndef HTTP_THINGS_H
#define HTTP_THINGS_H

#include <memory>
#include <string>
#include <vector>

#include "utils/buffer.h"


namespace srv {


struct HttpRequest {
    std::string request_type;
    std::string path;
    std::string protocol;

    std::vector<std::string> other;
};


class HttpResponce {
public:
    std::shared_ptr<Buffer> generate();

    std::string data = "HTTP/1.1 403 Forbidden\r\n\0";
};


}


#endif // HTTP_THINGS_H
