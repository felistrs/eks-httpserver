//
// Created by felistrs on 16.04.16.
//

#ifndef SERVER_EVENTLISTENER_H
#define SERVER_EVENTLISTENER_H


template <class T>
class EventListener {
public:
    virtual void onEvent(T * e = nullptr) = 0;
};


#include "utils/eventListener.inl.h"


#endif //SERVER_EVENTLISTENER_H
