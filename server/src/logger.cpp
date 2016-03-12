#include "utils/logger.h"

#include <iostream>


namespace srv {


void log(const std::string msg)
{
    std::clog << msg << std::endl;
}

void warning(const std::__cxx11::string msg)
{
    std::cerr << "Warning: " << msg << std::endl;
}

void error(const std::__cxx11::string msg)
{
    std::cerr << "Error: " << msg << std::endl;
}

template<class container>
void debug_hex(const container &buffer)
{
    using namespace std;

    cout << hex;

    for (auto c : buffer) {
        cout << unsigned(c) << " ";
    }
    cout << dec << endl;
}

template<class container>
void debug_string(const container &buffer)
{
    using namespace std;

    std::string str;
    for (auto c : buffer)
        if (isprint(c)) str += c;

    cout << "\"" << str << "\"" << endl;
}

template void debug_hex<std::string>(const std::string &);
template void debug_hex<std::vector<char>>(const std::vector<char> &);
template void debug_string<std::string>(const std::string &);
template void debug_string<std::vector<char>>(const std::vector<char> &);


}
