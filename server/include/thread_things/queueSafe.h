//
// Created by felistrs on 07.04.16.
//

#ifndef THREAD_THINGS__QUEUESAFE_H
#define THREAD_THINGS__QUEUESAFE_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "thread_things/threadTask.h"


template <class T>
class QueueSafe {
public:
    bool TryPop(T& item);
    void Pop_WithWait(T& item);
    std::queue<T> PopAll();

    void Push(const T& item);
    void Push(T&& item);

private:
    std::queue<T> _queue;
    std::mutex _lock;
    std::condition_variable _cond_var;
};

template class QueueSafe<ThreadTask>;
class Runnable;
template class QueueSafe<Runnable*>;


#endif //THREAD_THINGS__QUEUESAFE_H
