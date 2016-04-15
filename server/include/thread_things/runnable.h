#ifndef THREAD_THINGS___RUNNABLE_H
#define THREAD_THINGS___RUNNABLE_H


class Runnable {
public:
    Runnable() {}
    virtual ~Runnable() {}

    virtual void run() = 0;

public:
    int get_id() { return _id; }

private:
    int _id = 0;
};


#endif // THREAD_THINGS___RUNNABLE_H
