#include "commandProcessor.h"

#include "utils/logger.h"


namespace srv {


void CommandProcessor::add(std::unique_ptr<Command> command)
{
    _commands.push_back(std::move(command));
}

void CommandProcessor::processCommands()
{
    for (auto& command : _commands) {
        if (command) {
            command->run();
        } else {
            error("Command is empty !");
        }
    }

    _commands.clear();
}


}
