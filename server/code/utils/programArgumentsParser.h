//
// Created by felistrs on 10.04.16.
//

#ifndef UTILS__PROGRAM_ARGUMENTS_PARSER_H
#define UTILS__PROGRAM_ARGUMENTS_PARSER_H

#include <cassert>
#include <string>
#include <map>
#include <vector>


class ProgramArgumentsParser {
public:
    ProgramArgumentsParser(int argc, char **argv, std::map<std::string, std::string> def_args = {});

    void Parse();

    bool key_exists(std::string key);

    std::string AcquireValueByKey(std::string key, std::string default_value);
    int AcquireValueByKey(std::string key, int default_value);

public:
    std::map<std::string, std::string> get_paired_arguments() { assert(_parsed); return _paired_arguments; }
    std::vector<std::string> get_unpaired_arguments() { assert(_parsed); return  _unpaired_arguments; }

private:
    bool has_key_signature(std::string str);
    std::string extract_key(std::string str);

    int _argc;
    char **_argv;

    std::map<std::string, std::string> _paired_arguments;
    std::vector<std::string> _unpaired_arguments;

    bool _parsed = false;
};


#endif //UTILS__PROGRAM_ARGUMENTS_PARSER_H
