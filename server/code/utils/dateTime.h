//
// Created by felistrs on 03.04.16.
//

#ifndef SERVER_DATETIME_H
#define SERVER_DATETIME_H

#include <ctime>
#include <exception>
#include <string>


class TimeConverException : public std::exception
{
public:
    TimeConverException(std::string info) :
            info(info)
    {}

    std::string info;
};


tm GMTNow();

tm StringToTimeT(std::string str);
std::string TimeTToString(tm time);

#endif //SERVER_DATETIME_H
