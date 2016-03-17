#ifndef SOCKET_H
#define SOCKET_H


#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "sockets.h"


namespace srv {

using socket_t = int;

namespace sock {


socket_t SocketForLocalListening(int port);
void StartListening(socket_t descriptor, int max_connections);
socket_t Accept(socket_t descriptor);
void Close(socket_t descriptor);


}
}

#endif // SOCKET_H
