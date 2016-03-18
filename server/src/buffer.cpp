#include "utils/buffer.h"

#include <cstdio>

#include "utils/logger.h"


namespace srv {


Buffer::Buffer(std::vector<char>& data, int size) :
    _data(std::move(data)),
    _size(size),
    _position(0)
{
    if (_size < _data.size())
        warning("Buffer::Buffer : size of data is less than size arg.");
}

char Buffer::peek() const
{
    if (_position < _size) {
        return _data[_position];
    } else {
        return EOF;
    }
}

char Buffer::get()
{
    if (_position < _size) {
        char data = _data[_position];
        if (data == EOF) {
            warning("Buffer::get() : data == EOF");
        }
        _position++;
        return data;
    } else {
        return EOF;
    }
}

std::vector<char>& Buffer::data()
{
    return _data;
}

unsigned Buffer::size()
{
    return _size;
}


}
