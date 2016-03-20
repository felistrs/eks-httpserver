#ifndef UTILS__DATA_PROVIDER_H
#define UTILS__DATA_PROVIDER_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>


namespace srv {


class DataProvider {
public:
    DataProvider(std::string fpath); // init with file

    bool is_ready();

    std::pair<unsigned, char*> GetNextDataBlock(unsigned needed_size);
    bool is_end();

private:
    std::ifstream _fstream;

    bool _is_ready = false;
    bool _is_end = false;

    int _position = 0;
    std::vector<char> _buffer;
};


}

#endif // UTILS__DATA_PROVIDER_H
