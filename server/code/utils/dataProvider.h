#ifndef UTILS__DATA_PROVIDER_H
#define UTILS__DATA_PROVIDER_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>


class DataProvider {
public:
    DataProvider(std::string fpath); // init with file

    std::pair<unsigned, char*> GetNextDataBlock(unsigned needed_size);

public:
    bool is_end() { return _is_end; }
    bool is_ready() { return _is_ready; }


private:
    std::ifstream _fstream;

    int _position = 0;
    std::vector<char> _buffer;


    bool _is_end = false;
    bool _is_ready = false;
};


#endif // UTILS__DATA_PROVIDER_H
