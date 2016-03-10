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


}
