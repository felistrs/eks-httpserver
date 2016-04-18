//
// Created by felistrs on 16.04.16.
//

#include <algorithm>
#include <cassert>

#include "utils/eventProvider.h"
#include "utils/logger.h"


template <class T>
void EventProvider<T>::RegisterListener(EventListener<T> *listener) {
    assert(listener);

    auto it = std::find(_listeners.begin(), _listeners.end(), listener);
    if (it == _listeners.end()) {
        _listeners.push_back(listener);
    }
    else {
        warning("EventHandler::RegisterListener: Listener already exists");
    }
}

template <class T>
void EventProvider<T>::UnregisterListener(EventListener<T> *listener) {
    assert(listener);

    auto it = std::find(_listeners.begin(), _listeners.end(), listener);
    if (it != _listeners.end()) {
        _listeners.erase(it);
    }
    else {
        warning("EventHandler::UnregisterListener: Listener does not exists");
    }
}

template <class T>
void EventProvider<T>::onEvent() {
    for (auto listener : _listeners) {
        listener->onEvent();
    }
}
