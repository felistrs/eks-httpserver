//
// Created by felistrs on 18.04.16.
//

#ifndef SERVER_QUEUESAFE_INL_H
#define SERVER_QUEUESAFE_INL_H

#include "queueSafe.h"


#include "thread_things/threadTask.h"
template class QueueSafe<ThreadTask>;

#include "serverRunnable.h"
template class QueueSafe<server::ServerRunnable*>;


#endif //SERVER_QUEUESAFE_INL_H
