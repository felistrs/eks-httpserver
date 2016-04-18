//
// Created by felistrs on 18.04.16.
//

#ifndef SERVER_QUEUESAFE_INL_H
#define SERVER_QUEUESAFE_INL_H

#include "thread_things/queueSafe.h"


#include "thread_things/threadTask.h"
template class QueueSafe<ThreadTask>;

#include "thread_things/runnable.h"
template class QueueSafe<Runnable*>;


#endif //SERVER_QUEUESAFE_INL_H
