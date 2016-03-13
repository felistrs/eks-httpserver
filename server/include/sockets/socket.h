#ifndef SOCKET_H
#define SOCKET_H


#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "sockets.h"


namespace srv {


using socket_t = int;


class Socket
{
public:
    Socket(const Socket& s) = delete;
    Socket& operator =(const Socket s) = delete;

    static socket_t SocketForLocalListening(int port);
    static void StartListening(socket_t descriptor, int max_connections);
    static socket_t Accept(socket_t descriptor);
    static void Close(socket_t descriptor);
};


}

#endif // SOCKET_H
