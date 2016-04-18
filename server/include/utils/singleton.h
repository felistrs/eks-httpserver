//
// Created by felistrs on 16.04.16.
//

#ifndef SERVER_SINGLETON_H
#define SERVER_SINGLETON_H

#include "utils/logger.h"
#include "utils/singletonContainer.h"


class Singleton {
public:
    Singleton(bool release_by_singleton_container = true)
    {
        if (release_by_singleton_container) {
            SingletonContainer::RegisterSingleton(this);
        }
    }

    virtual ~Singleton() {}
};


#endif //SERVER_SINGLETON_H
