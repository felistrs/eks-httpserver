#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server.h"


class HttpServer : public Server
{
public:
    HttpServer();
    virtual ~HttpServer();

    void on_connect(int conn_fd);

private:

};

#endif // HTTPSERVER_H
