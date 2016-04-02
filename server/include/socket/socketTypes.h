#ifndef SOCKETS__SOCKET_TYPE_H
#define SOCKETS__SOCKET_TYPE_H

#include <exception>
#include <map>
#include <string>


namespace server {



using connection_handler = int;

const connection_handler CONNECTION_HANDLER_INVALID = -1;


struct connection_descriptor_ {
    enum class Type { CNone, CSocket };

    Type type = Type::CNone;
    int sock_handler = 0;
};


struct connection_descriptor {  // TODO: rem
    connection_handler sock = 0;
    int state = 0;
};



class ConnectionException : public std::exception
{
public:
    ConnectionException(std::string info) :
        info(info)
    {}

    std::string info;
};


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


connection_handler generateConnectionHandler(int sock); // TODO: ???
void releaseConnectionHandler(connection_handler handler);
bool testIfAllHandlersReleased();

connection_descriptor_* GetConnectionDescriptor(connection_handler handler);


}

#endif // SOCKETS__SOCKET_TYPE_H
