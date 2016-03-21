#ifndef UTILS__LOGGER_H
#define UTILS__LOGGER_H

#include <string>
#include <vector>


void log(const std::string msg);
void warning(const std::string msg);
void error(const std::string msg);

template<class container>
void debug_hex(const container &buffer);

template<class container>
void debug_string(const container &buffer);


#endif // UTILS__LOGGER_H
