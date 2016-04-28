#ifndef CONNECTION__FUNCTIONS_H
#define CONNECTION__FUNCTIONS_H

#include <vector>

#include "types.h"


class DataBuffer;


namespace server {


// Connections
std::pair<connection_handler, connection_descriptor*> GenerateConnectionHandler();
void ReleaseConnectionHandler(connection_handler handler);

void ForEachConnection(std::function<void(connection_handler, connection_descriptor *)>);

connection_descriptor* GetConnectionDescriptor(connection_handler handler);


// Operations on connection
connection_handler CreateConnectionForServer(int port);
void StartListening(connection_handler descriptor, int max_connections);
connection_handler AcceptNewConnection(connection_handler descriptor);
void CloseConnection(connection_handler descriptor);

void ObtainIdleConnections(
        const std::vector<connection_handler>& connections,
        std::vector<connection_handler> *read_out,
        std::vector<connection_handler> *write_out,
        std::vector<connection_handler> *exception_out);

void WriteBuffer(connection_handler sock, DataBuffer *buffer);
DataBuffer ReadBuffer(connection_handler sock);


}


#endif // CONNECTION__FUNCTIONS_H
