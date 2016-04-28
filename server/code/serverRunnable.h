//
// Created by felistrs on 26.04.16.
//

#ifndef SERVER_SERVERRUNNABLE_H
#define SERVER_SERVERRUNNABLE_H

#include "connection/types.h"
#include "thread_things/runnable.h"


namespace server
{


class ServerRunnable: public Runnable
{
public:
    ServerRunnable(connection_handler connection) :
        _connection(connection)
    {}

    virtual ~ServerRunnable() {}

    virtual void OnBeforeScheduling();
    virtual void OnFinished();

protected:
    connection_handler _connection;
};


}

#endif //SERVER_SERVERRUNNABLE_H
