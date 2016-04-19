#include <cstdio>
#include <utility>

#include "dataBuffer.h"
#include "logger.h"


DataBuffer::DataBuffer(DataBuffer &&buff) :
    _size(buff._size),
    _position(buff._position),
    _data(std::move(buff._data))
{}

DataBuffer::DataBuffer(std::vector<char>&& data, size_t size) :
    _data(data),
    _size(size),
    _position(0)
{
    if (_size < _data.size())
        warning("Buffer::Buffer : size of data is less than size arg.");
}

std::pair<char, bool> DataBuffer::peek_byte() const
{
    if (_position < _size) {
        return std::make_pair(_data[_position], true);
    } else {
        return std::make_pair(0, false);
    }
}

std::pair<char, bool> DataBuffer::read_byte()
{
    auto res = peek_byte();
    if (res.second) {
        _position++;
    }
    return res;
}
