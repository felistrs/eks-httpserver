#ifndef SOCKETS__SOCKET_H
#define SOCKETS__SOCKET_H

#include <vector>

#include "socket/socketTypes.h"

#include "utils/buffer.h"


namespace server { namespace sock {


connection_handler CreateSocketForServer(int port);
void StartListening(connection_handler descriptor, int max_connections);
connection_handler AcceptNewConnection(connection_handler descriptor);
void CloseSocket(connection_handler descriptor);

void ObtainIdleSockets(
        const std::vector<connection_handler>& connections,
        std::vector<connection_handler> *read_out,
        std::vector<connection_handler> *write_out,
        std::vector<connection_handler> *exception_out);

void SendBuffer(connection_handler sock, Buffer* buffer);
Buffer RecvBuffer(connection_handler sock);


} }


#endif // SOCKETS__SOCKET_H
