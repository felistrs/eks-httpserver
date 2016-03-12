#ifndef COMMAND_PROCESSOR_INTERFACE_H
#define COMMAND_PROCESSOR_INTERFACE_H

#include <vector>
#include <memory>

#include "command.h"


namespace srv {


class CommandProcessor
{
public:
    CommandProcessor() {}
    virtual ~CommandProcessor() {}

    void add(std::unique_ptr<Command> command);
    void processCommands();

private:
    std::vector<std::unique_ptr<Command>> _commands;
};


}

#endif // COMMAND_PROCESSOR_INTERFACE_H
