//
// Created by felistrs on 07.04.16.
//

#include <iostream>
#include <utils/logger.h>

#include "thread_things/queueSafe.h"
#include "thread_things/threadPool.h"


ThreadPool::ThreadPool(TaskRunner task_runner_func) :
    _task_runner(task_runner_func)
{}


void ThreadPool::ThreadWorkerFunction(ThreadPool::WorkerContext *context)
{
    log("Thread start : " + std::to_string(context->id) );

    ThreadTask* task = nullptr;

    while (!context->join_flag)
    {
        if (task == nullptr) {
            context->tasks_queue->Pop_WithWait(task);
            log("Thread " + std::to_string(context->id) + " picked task : " + std::to_string(task->id) );
        }

        context->task_runner(task);

        if (task->completed)
        {
            context->completed_queue->Push(task);
            task = nullptr;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // TODO: 50 ms ?
    }

    if (task != nullptr)
    {
        log("On thread " + std::to_string(context->id) + " task wasn\'t completed : " + std::to_string(task->id) );
        context->tasks_queue->Push(task);
    }

    log("Thread finish : " + std::to_string(context->id) );
};


void ThreadPool::Start(unsigned thread_count)
{
    _thread_func_context = std::vector<WorkerContext>(thread_count);

    for (unsigned i = 0; i < thread_count; ++i) {
        // Context
        _thread_func_context[i].id = i;
        _thread_func_context[i].tasks_queue = &_tasks_queue;
        _thread_func_context[i].completed_queue = &_completed_tasks;
        _thread_func_context[i].task_runner = _task_runner;

        // Start thread
        _threads.push_back(
                std::thread(ThreadWorkerFunction, _thread_func_context.data() + i)
        );
    }
}

void ThreadPool::PushTask(ThreadTask* task)
{
    _tasks_queue.Push(task);
}

std::queue<ThreadTask*> ThreadPool::PopCompletedTasks()
{
    return  _completed_tasks.PopAll();
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

ThreadPool::~ThreadPool() {
    JoinAllThreads();
}
