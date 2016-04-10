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



class ThreadPool {
public:
    using TaskRunner = std::function<void(ThreadTask*)>;

    struct WorkerContext
    {
        int id = -1;
        QueueSafe<ThreadTask*> *tasks_queue = nullptr;
        QueueSafe<ThreadTask*> *completed_queue = nullptr;
        bool join_flag = false;
        TaskRunner task_runner;
    };


    ThreadPool(TaskRunner task_runner_func);
    ~ThreadPool();


    void Start(unsigned thread_count = std::thread::hardware_concurrency() - 1);

    void PushTask(ThreadTask* task);
    std::queue<ThreadTask*> PopCompletedTasks();

    void JoinAllThreads();

protected:
    static void ThreadWorkerFunction(ThreadPool::WorkerContext *context);

private:
    std::vector<std::thread> _threads;
    std::vector<WorkerContext> _thread_func_context;

    QueueSafe<ThreadTask*> _tasks_queue;
    QueueSafe<ThreadTask*> _completed_tasks;

    TaskRunner _task_runner;
};


#endif // THREAD_THINGS__THREADPOOL_H
