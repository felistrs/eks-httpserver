#include "httpThings.h"


namespace srv {


std::shared_ptr<srv::Buffer> srv::HttpResponce::generate()
{
    std::vector<char> v;
    std::copy(data.begin(), data.end(), std::back_inserter(v));
    v.push_back(0);
    return std::shared_ptr<Buffer>(new Buffer(v, v.size()));
}


}
