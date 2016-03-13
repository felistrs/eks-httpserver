#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>


class Command
{
public:
    Command() {}
    virtual ~Command() {}

    virtual void run() = 0;
};

#endif // COMMAND_H
