#ifndef SOCKETS__SOCKET_TYPE_H
#define SOCKETS__SOCKET_TYPE_H

#include <exception>
#include <map>
#include <string>


namespace server {


using socket_handler = int;

struct connection_descriptor {
    socket_handler sock = 0;
    int state = 0;
};

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
    SocketException(std::string info, unsigned err_code, std::string err_str) :
        info(info),
        _error_code(err_code),
        _error_str(err_str)
    {}

    std::string info;
    unsigned _error_code;
    std::string _error_str;
};


}

#endif // SOCKETS__SOCKET_TYPE_H
