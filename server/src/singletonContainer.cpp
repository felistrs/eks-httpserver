//
// Created by felistrs on 16.04.16.
//

#include <cassert>

#include "utils/singleton.h"
#include "utils/singletonContainer.h"


SingletonContainer* SingletonContainer::_container = nullptr;


SingletonContainer* SingletonContainer::AcquireInstance()
{
    return _container ? _container : (_container = new SingletonContainer());
}

void SingletonContainer::RegisterSingleton(Singleton* singleton)
{
    assert(singleton);
    auto container = AcquireInstance();
    container->register_singleton(singleton);
}

void SingletonContainer::ReleaseAllSingletones()
{
    if (_container) {
        _container->release_all_singletones();
    }
}

void SingletonContainer::register_singleton(Singleton* singleton)
{
    _singletons_vector.push_back(singleton);
}

void SingletonContainer::release_all_singletones()
{
    for (auto singleton : _singletons_vector) {
        delete singleton;
    }
    _singletons_vector.clear();
}

void SingletonContainer::ReleaseSingletonContainer()
{
    if (_container) {
        delete _container;
        _container = nullptr;
    }
}
