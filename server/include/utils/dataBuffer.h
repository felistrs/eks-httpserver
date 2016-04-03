#ifndef UTILS__BUFFER_H
#define UTILS__BUFFER_H

#include <vector>


class DataBuffer
{
public:
    DataBuffer() {}
    DataBuffer(DataBuffer&& buff);
    DataBuffer(std::vector<char>&& data, int size);

    DataBuffer(const DataBuffer&) = delete;
    DataBuffer& operator=(const DataBuffer&) = delete;

    std::pair<char, bool> peek_byte() const;
    std::pair<char, bool> read_byte();

public:
    const std::vector<char>& data() const { return _data; }
    unsigned size() { return _size; }

private:
    int _size = 0;
    std::vector<char> _data;

    int _position;
};


#endif // UTILS__BUFFER_H
