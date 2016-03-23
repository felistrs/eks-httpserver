#ifndef SOCKETS__SOCKET_TYPE_H
#define SOCKETS__SOCKET_TYPE_H

#include <exception>
#include <string>


namespace server {


using socket_handler = int;


class SocketException : public std::exception
{
public:
    SocketException(std::string info) :
        info(info) {}

    std::string info;
};


}

#endif // SOCKETS__SOCKET_TYPE_H
