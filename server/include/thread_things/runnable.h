#ifndef THREAD_THINGS___RUNNABLE_H
#define THREAD_THINGS___RUNNABLE_H


class Runnable {
public:
    virtual ~Runnable() {}

    virtual void run() = 0;
};


#endif // THREAD_THINGS___RUNNABLE_H
