//
// Created by felistrs on 09.04.16.
//

#ifndef SERVER_THREADTASK_H_H
#define SERVER_THREADTASK_H_H


#include "runnable.h"
#include "runnable.h"


struct ThreadTask
{
    using RunnableDoneCallback = std::function<void(Runnable*)>;

    ThreadTask() {}
    ThreadTask(Runnable* runnable, RunnableDoneCallback callback) :
            runnable(runnable),
            done_callback(callback)
    {}
    virtual ~ThreadTask() {}

//    long id = 0;
//    bool completed = false;

    Runnable* runnable;
    RunnableDoneCallback done_callback;
};


#endif //SERVER_THREADTASK_H_H
