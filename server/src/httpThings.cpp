#include "httpThings.h"


namespace server {


Buffer server::HttpResponse::generate()
{
    std::vector<char> v;
    std::copy(data.begin(), data.end(), std::back_inserter(v));
    v.push_back(0);
    return Buffer(std::move(v), v.size());
}


}
