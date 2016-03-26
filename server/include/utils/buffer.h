#ifndef UTILS__BUFFER_H
#define UTILS__BUFFER_H

#include <vector>


class Buffer
{
public:
    Buffer() {}
    Buffer(Buffer&& buff);
    Buffer(std::vector<char>&& data, int size);

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;


    char peek() const;
    char get();

    std::vector<char>& data();
    unsigned size();

private:
    int _size = 0;
    std::vector<char> _data;

    int _position;
};


#endif // UTILS__BUFFER_H
