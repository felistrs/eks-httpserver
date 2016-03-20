#include "utils/dataProvider.h"


namespace srv {


DataProvider::DataProvider(std::string fpath)
{
    _fstream = std::ifstream(fpath);
    if (_fstream) {
        _is_ready = _fstream.is_open();
    }
}

bool DataProvider::is_ready()
{
    return _is_ready;
}

std::pair<unsigned, char *> DataProvider::GetNextDataBlock(unsigned needed_size)
{
    if (!_is_ready || _is_end)
    {
        return std::pair<unsigned, char *>(0, nullptr);
    }
    else
    {
        _buffer.resize(needed_size);
        _fstream.read(_buffer.data(), needed_size); // TODO: add read size test

        _is_end = _fstream.eof();

        return std::pair<unsigned, char *>(needed_size, _buffer.data());
    }
}

bool DataProvider::is_end()
{
    return _is_end;
}


}
