#ifndef SOCKETS__SOCKET_TYPE_H
#define SOCKETS__SOCKET_TYPE_H

#include <exception>
#include <map>
#include <string>


namespace server {


using socket_handler = int;


//using socket_handler = int;

//struct socket_descriptor {
//    enum class Type { CNone, CSocket };

//    Type type = Type::CNone;
//    int sock_handler = 0;
//};


//socket_handler generateSocketHandler();
//void releaseSocketHandler(socket_handler handler);
//bool testIfAllHandlersReleased();


class SocketException : public std::exception
{
public:
    SocketException(std::string info) :
        info(info) {}

    std::string info;
};


}

#endif // SOCKETS__SOCKET_TYPE_H
