#ifndef SOCKETS__SOCKET_H
#define SOCKETS__SOCKET_H

#include <memory>

#include "socket.h"
#include "socket/socketTypes.h"
#include "utils/buffer.h"


namespace server { namespace sock {


socket_handler CreateSocketForServer(int port);
void StartListening(socket_handler descriptor, int max_connections);
socket_handler AcceptNewConnection(socket_handler descriptor);
void CloseSocket(socket_handler descriptor);

void SendBuffer(socket_handler sock, std::shared_ptr<Buffer> buffer);
std::shared_ptr<Buffer> RecvBuffer(socket_handler sock);


} }


#endif // SOCKETS__SOCKET_H
