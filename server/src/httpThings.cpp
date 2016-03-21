#include "httpThings.h"


namespace server {


std::shared_ptr<Buffer> server::HttpResponse::generate()
{
    std::vector<char> v;
    std::copy(data.begin(), data.end(), std::back_inserter(v));
    v.push_back(0);
    return std::shared_ptr<Buffer>(new Buffer(std::move(v), v.size()));
}


}
