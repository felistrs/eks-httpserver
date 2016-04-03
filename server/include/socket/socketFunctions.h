#ifndef SOCKETS__SOCKET_H
#define SOCKETS__SOCKET_H

#include <vector>

#include "socket/socketTypes.h"

#include "utils/dataBuffer.h"


namespace server { namespace sock {


connection_handler CreateConnectionForServer(int port);
void StartListening(connection_handler descriptor, int max_connections);
connection_handler AcceptNewConnection(connection_handler descriptor);
void CloseConnection(connection_handler descriptor);

void ObtainIdleConnections(
        const std::vector<connection_handler>& connections,
        std::vector<connection_handler> *read_out,
        std::vector<connection_handler> *write_out,
        std::vector<connection_handler> *exception_out);

void SendBuffer(connection_handler sock, DataBuffer* buffer);
DataBuffer RecvBuffer(connection_handler sock);


} }


#endif // SOCKETS__SOCKET_H
