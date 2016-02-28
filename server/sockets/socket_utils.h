#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <exception>
#include <string>


class SocketException : public std::exception
{
public:
    SocketException(std::string info) :
        info(info) {}

public:
    std::string info;
};

#endif // SOCKET_UTILS_H
