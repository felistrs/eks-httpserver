#ifndef SOCKETS__SOCKET_H
#define SOCKETS__SOCKET_H


#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "sockets.h"
#include "sockets/socketTypes.h"


namespace server { namespace sock {


socket_handler CreateSocketForServer(int port);
void StartListening(socket_handler descriptor, int max_connections);
socket_handler AcceptNewConnection(socket_handler descriptor);
void CloseSocket(socket_handler descriptor);


} }


#endif // SOCKETS__SOCKET_H
