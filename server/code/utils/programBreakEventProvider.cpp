//
// Created by felistrs on 16.04.16.
//

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "programBreakEventProvider.h"
#include "logger.h"


ProgramBreakEventProvider* ProgramBreakEventProvider::event_provider = nullptr;


ProgramBreakEventProvider::ProgramBreakEventProvider()
{
    // init event_provider
    if (event_provider) {
        error("ProgramBreakEventProvider was intantiated");
    }
    else {
        event_provider = this;
    }

    struct sigaction signal_handler;

    signal_handler.sa_handler = [] (int s) {
        event_provider->RunProramBreakEvent();
    };

    sigemptyset(&signal_handler.sa_mask);
    signal_handler.sa_flags = 0;

    sigaction(SIGINT, &signal_handler, NULL);
}

ProgramBreakEventProvider::~ProgramBreakEventProvider()
{
    event_provider = nullptr;
}

void ProgramBreakEventProvider::RunProramBreakEvent()
{
    onEvent();
}
