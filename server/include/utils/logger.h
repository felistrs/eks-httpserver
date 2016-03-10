#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <string>

namespace srv {


void log(const std::string msg);
void warning(const std::string msg);
void error(const std::string msg);


}

#endif // UTILS_LOGGER_H
