#include "httpServer.h"

#include <cstring>


HttpServer::HttpServer()
{

}

HttpServer::~HttpServer()
{

}

void HttpServer::on_connect(int conn_fd)
{
    char *msg = "Test message";
    int len, bytes_sent;

    len = strlen(msg);
    bytes_sent = send(conn_fd, msg, len, 0);
}
