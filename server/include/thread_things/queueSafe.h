//
// Created by felistrs on 07.04.16.
//

#ifndef THREAD_THINGS__QUEUESAFE_H
#define THREAD_THINGS__QUEUESAFE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


template <class T>
class QueueSafe {
public:
//    T Pop_WithWait();
    void Pop_WithWait(T& item);
    std::queue<T> PopAll();

    void Push(const T& item);
    void Push(T&& item);

private:
    std::queue<T> _queue;
    std::mutex _lock;
    std::condition_variable _cond_var;
};


class ThreadTask; // TODO: move to other file ?
template
class QueueSafe<ThreadTask*>;


#endif //THREAD_THINGS__QUEUESAFE_H
