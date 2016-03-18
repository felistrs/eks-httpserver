#ifndef UTILS__BUFFER_H
#define UTILS__BUFFER_H

#include <vector>


namespace srv {


class Buffer
{
public:
    // Moves ovnership of data
    Buffer(std::vector<char>& data, int size);

    char peek() const;
    char get();

    std::vector<char>& data();
    unsigned size();

private:
    int _size = 0;
    std::vector<char> _data;

    int _position;
};


}

#endif // UTILS__BUFFER_H
