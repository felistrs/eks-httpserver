#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <exception>
#include <string>


namespace srv {


class SocketException : public std::exception
{
public:
    SocketException(std::string info) :
        info(info) {}


    std::string info;
};


}

#endif // SOCKET_UTILS_H
