#ifndef CONNECTION__TYPES_H
#define CONNECTION__TYPES_H

#include <exception>
#include <map>
#include <string>
#include <memory>


namespace server {


class communication_type;


using connection_handler = int;
const connection_handler CONNECTION_HANDLER_INVALID = -1;


struct connection_descriptor {
    enum class Type { CNone, CSocket };

    Type type = Type::CNone;
    int sock_handler = 0;
    int state = 0;

    bool _access_lock = false;

    std::unique_ptr<communication_type> request;
};


class communication_type {
public:
    virtual ~communication_type() {}
};


class ConnectionException : public std::exception
{
public:
    ConnectionException(std::string info) :
        info(info)
    {}

    virtual const char* what() const throw()
    {
        return info.c_str();
    }

private:
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

    virtual const char* what() const throw()
    {
        return info.c_str();
    }

    unsigned _error_code;
    std::string _error_str;

private:
    std::string info;
};


}

#endif // CONNECTION__TYPES_H
