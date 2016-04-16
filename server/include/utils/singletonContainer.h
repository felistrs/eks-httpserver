//
// Created by felistrs on 16.04.16.
//

#ifndef SERVER_SINGLETONCONTAINER_H
#define SERVER_SINGLETONCONTAINER_H

#include <vector>


class Singleton;

class SingletonContainer {
public:
    static void RegisterSingleton(Singleton* singleton);
    static void ReleaseAllSingletones();
    static void ReleaseSingletonContainer();

private:
    SingletonContainer() {}
    ~SingletonContainer() {}

    static SingletonContainer* AcquireInstance();
    void register_singleton(Singleton* singleton);
    void release_all_singletones();

    void ReleaseSingletones();

    static SingletonContainer* _container;

    std::vector<Singleton*> _singletons_vector;
};


#endif //SERVER_SINGLETONCONTAINER_H
