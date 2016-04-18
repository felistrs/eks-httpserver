#include <iostream>
#include <utils/logger.h>

#include "thread_things/queueSafe.h"
#include "thread_things/threadPool.h"


ThreadPool* ThreadPool::s_default_thread_pool = nullptr;


void ThreadPool::ThreadWorkerFunction(ThreadPool::WorkerContext *context)
{
    log("Thread start : " + std::to_string(context->id) );

    while (!context->join_flag)
    {
        ThreadTask task;

        if (context->tasks_queue->TryPop(task)) {
            //context->tasks_queue->Pop_WithWait(task);
            log("Thread " + std::to_string(context->id) + " picked task : "); // + std::to_string(task->id) );

            task.runnable->run();
            task.done_callback(task.runnable);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(c_worker_task_wait_ms));
        }
    }

    log("Thread finish : " + std::to_string(context->id) );
};


void ThreadPool::Start(unsigned thread_count)
{
    _thread_func_context = std::vector<WorkerContext>(thread_count);

    for (unsigned i = 0; i < thread_count; ++i) {
        // Context
        _thread_func_context[i].id = i;
        _thread_func_context[i].tasks_queue = &_runnable_queue;

        // Start thread
        _threads.push_back(
                std::thread(ThreadWorkerFunction, _thread_func_context.data() + i)
        );
    }
}

void ThreadPool::ScheduleExecutionRunnable(Runnable *runnable,
                                           ThreadTask::RunnableDoneCallback done_callback)
{
    _runnable_queue.Push(ThreadTask(runnable, done_callback));
}

void ThreadPool::JoinAllThreads()
{
    for (auto& context : _thread_func_context)
        context.join_flag = true;

    for (auto& thread : _threads)
        thread.join();

    _threads.clear();
    _thread_func_context.clear();
}

ThreadPool *ThreadPool::ReceiveDefault() {
    if (s_default_thread_pool) {
        return s_default_thread_pool;
    }
    else {
        return s_default_thread_pool = new ThreadPool();
    }
}


ThreadPool::~ThreadPool() {
    JoinAllThreads();
}
