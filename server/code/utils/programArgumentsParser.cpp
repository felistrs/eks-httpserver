//
// Created by felistrs on 10.04.16.
//

#include "programArgumentsParser.h"

#include "logger.h"


ProgramArgumentsParser::ProgramArgumentsParser(int argc, char **argv, std::map<std::string, std::string> def_args) :
    _argc(argc),
    _argv(argv),
    _paired_arguments(def_args)
{}

void ProgramArgumentsParser::Parse()
{
    for (int i = 1; i < _argc; ++i)
    {
        std::string str(_argv[i]);

        if (has_key_signature(str)) {
            std::string key = extract_key(str);
            std::string value;

            if (i != _argc - 1)
            {
                std::string str_next(_argv[i + 1]);
                if (!has_key_signature(str_next)) {
                    value = str_next;
                    ++i;
                }
            }

            _paired_arguments[key] = value;
        }
        else {
            _unpaired_arguments.push_back(str);
        }
    }

    _parsed = true;
}

bool ProgramArgumentsParser::key_exists(std::string key)
{
    return _paired_arguments.find(key) != _paired_arguments.end();
}

std::string ProgramArgumentsParser::AcquireValueByKey(std::string key, std::string default_value)
{
    assert(_parsed);

    auto it = _paired_arguments.find(key);
    if (it == _paired_arguments.end()) {
        return default_value;
    }
    else {
        return it->second;
    }
}

int ProgramArgumentsParser::AcquireValueByKey(std::string key, int default_value)
{
    assert(_parsed);

    int value = default_value;

    auto it = _paired_arguments.find(key);
    if (it != _paired_arguments.end()) {
        try {
            value = std::stoi(it->second);
        }
        catch (...) {
            error("(ProgramArgumentsParser::AcquireValueByKey) Error parsing int : " + it->second);
        }
    }

    return value;
}

bool ProgramArgumentsParser::has_key_signature(std::string str)
{
    bool result = false;

    if (str.size())
    {
        result = str[0] == '-' && str.back() != '-';
    }

    return result;
}

std::string ProgramArgumentsParser::extract_key(std::string str)
{
    std::string result;

    auto pos = str.find_first_not_of('-');
    if (pos != std::string::npos) {
        result = str.substr(pos);
    }

    return result;
}
