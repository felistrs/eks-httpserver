#ifndef SOCKETS__SOCKET_TYPE_H
#define SOCKETS__SOCKET_TYPE_H

#include <exception>
#include <map>
#include <string>
#include <memory>


namespace server {



using connection_handler = int;
const connection_handler CONNECTION_HANDLER_INVALID = -1;


enum class EConnectionType { ERead, EWrite, EException };


class communication_type {
public:
    virtual ~communication_type() {}
};


struct connection_descriptor {
    enum class Type { CNone, CSocket };

    Type type = Type::CNone;
    int sock_handler = 0;
    int state = 0;

    bool _access_lock = false;

    std::unique_ptr<communication_type> request;
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


std::pair<connection_handler, connection_descriptor*> generateConnectionHandler();
void releaseConnectionHandler(connection_handler handler);
bool testIfAllHandlersReleased();

void forEachConnection(std::function<void(connection_handler, connection_descriptor *)>);

connection_descriptor* GetConnectionDescriptor(connection_handler handler);


}

#endif // SOCKETS__SOCKET_TYPE_H
