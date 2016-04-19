//
// Created by felistrs on 16.04.16.
//

#ifndef SERVER_EVENTPROVIDER_H
#define SERVER_EVENTPROVIDER_H

#include "eventProvider.h"
#include "eventListener.h"

#include <vector>


template <class T>
class EventProvider {
public:
    void RegisterListener(EventListener<T> *listener);
    void UnregisterListener(EventListener<T> *listener);


protected:
    void onEvent();


private:
    std::vector<EventListener<T>*> _listeners;
};


class ProgramBreakEvent;
template class EventProvider<ProgramBreakEvent>;


#endif //SERVER_EVENTPROVIDER_H
