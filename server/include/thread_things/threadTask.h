//
// Created by felistrs on 09.04.16.
//

#ifndef SERVER_THREADTASK_H_H
#define SERVER_THREADTASK_H_H


struct ThreadTask
{
    virtual ~ThreadTask() {}

    long id = 0;
    bool completed = false;
};


#endif //SERVER_THREADTASK_H_H
