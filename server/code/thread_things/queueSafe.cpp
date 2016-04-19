#include <queue>
#include <condition_variable>
#include <thread>

#include "queueSafe.h"


template <class T>
void QueueSafe<T>::Pop_WithWait(T &item) {
    std::unique_lock<std::mutex> ulock(_lock);
    while (_queue.empty())
    {
        _cond_var.wait(ulock);
    }
    item = _queue.front();
    _queue.pop();
}

template <class T>
void QueueSafe<T>::Push(const T &item) {
    std::unique_lock<std::mutex> ulock(_lock);
    _queue.push(item);
    ulock.unlock();
    _cond_var.notify_one();
}

template <class T>
void QueueSafe<T>::Push(T &&item) {
    std::unique_lock<std::mutex> ulock(_lock);
    _queue.push(std::move(item));
    ulock.unlock();
    _cond_var.notify_one();
}

template <class T>
std::queue<T> QueueSafe<T>::PopAll() {
    std::unique_lock<std::mutex> ulock(_lock);
    std::queue<T> ret;
    std::swap(_queue, ret);
    return ret;
}

template <class T>
bool QueueSafe<T>::TryPop(T &item) {
    std::unique_lock<std::mutex> ulock(_lock);

    if (_queue.size()) {
        item = _queue.front();
        _queue.pop();
        return true;
    }
    else {
        return false;
    }
}

