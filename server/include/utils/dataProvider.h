#ifndef UTILS__DATA_PROVIDER_H
#define UTILS__DATA_PROVIDER_H

#include <string>


namespace srv {


class DataProvider {
public:
    DataProvider(std::string fpath); // init with file
    ~DataProvider();
};


}

#endif // UTILS__DATA_PROVIDER_H
