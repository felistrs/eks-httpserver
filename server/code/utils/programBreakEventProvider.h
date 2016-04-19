//
// Created by felistrs on 16.04.16.
//

#ifndef SERVER_PROGRAMBREAKEVENTPROVIDER_H
#define SERVER_PROGRAMBREAKEVENTPROVIDER_H

#include "eventProvider.h"


struct ProgramBreakEvent {};


class ProgramBreakEventProvider : public EventProvider<ProgramBreakEvent> {
public:
    ProgramBreakEventProvider();
    virtual ~ProgramBreakEventProvider();

    void RunProramBreakEvent();

public:
    static ProgramBreakEventProvider* event_provider;
};


#endif //SERVER_PROGRAMBREAKEVENTPROVIDER_H
