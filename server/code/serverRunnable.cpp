//
// Created by felistrs on 26.04.16.
//

#include "serverRunnable.h"
#include "socket/socketTypes.h"


namespace server {


void ServerRunnable::OnBeforeScheduling()
{
    auto descr = GetConnectionDescriptor(_connection);
    if (descr == nullptr) {
        throw ConnectionException("(ServerRunnable::OnBeforeScheduling) : Bad descriptor; " + std::to_string(get_id()));
    }

    descr->_access_lock = true;
}

void ServerRunnable::OnFinished()
{
    auto descr = GetConnectionDescriptor(_connection);
    if (descr == nullptr) {
        throw ConnectionException("(ServerRunnable::OnFinished) : Bad descriptor; //" + std::to_string(get_id()));
    }

    descr->_access_lock = false;
}


}
