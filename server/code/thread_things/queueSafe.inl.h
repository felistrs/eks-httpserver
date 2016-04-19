//
// Created by felistrs on 18.04.16.
//

#ifndef SERVER_QUEUESAFE_INL_H
#define SERVER_QUEUESAFE_INL_H

#include "queueSafe.h"


#include "threadTask.h"
template class QueueSafe<ThreadTask>;

#include "runnable.h"
template class QueueSafe<Runnable*>;


#endif //SERVER_QUEUESAFE_INL_H
