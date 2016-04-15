//
// Created by felistrs on 07.04.16.
//

#ifndef THREAD_THINGS__THREADPOOL_H
#define THREAD_THINGS__THREADPOOL_H

#include <functional>
#include <thread>
#include <vector>

#include "thread_things/queueSafe.h"
#include "thread_things/threadTask.h"
#include "thread_things/runnable.h"



class ThreadPool {
public:
    struct WorkerContext
    {
        int id = -1;
        QueueSafe<ThreadTask> *tasks_queue = nullptr;
        bool join_flag = false;
    };


    ~ThreadPool();

    void Start(unsigned thread_count = std::thread::hardware_concurrency() - 1);

    void ScheduleExecutionRunnable(Runnable *runnable, ThreadTask::RunnableDoneCallback done_callback);

    void JoinAllThreads();

protected:
    static void ThreadWorkerFunction(ThreadPool::WorkerContext *context);

private:
//    ThreadPool() {}


    std::vector<std::thread> _threads;
    std::vector<WorkerContext> _thread_func_context;

    QueueSafe<ThreadTask> _runnable_queue;
};


#endif // THREAD_THINGS__THREADPOOL_H
