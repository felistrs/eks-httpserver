//
// Created by felistrs on 03.04.16.
//

#include <algorithm>
#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "utils/dateTime.h"

const std::map<int, std::string> c_wday_id_to_short_name =
{
        { 0, "Sun" },
        { 1, "Mon" },
        { 2, "Tue" },
        { 3, "Wed" },
        { 4, "Thu" },
        { 5, "Fri" },
        { 6, "Sat" }
};

const std::map<std::string, int> c_wday_name_to_id =
{
        { "Sun", 0 },
        { "Sunday", 0 },
        { "Mon", 1 },
        { "Monday", 1 },
        { "Tue", 2 },
        { "Tuesday", 2 },
        { "Wed", 3 },
        { "Wednesday", 3 },
        { "Thu", 4 },
        { "Thursday", 4 },
        { "Fri", 5 },
        { "Friday", 5 },
        { "Sat", 6 },
        { "Saturday", 6 }
};

const std::map<int, std::string> c_month_id_to_name =
{
        { 0, "Jan" },
        { 1, "Feb" },
        { 2, "Mar" },
        { 3, "Apr" },
        { 4, "May" },
        { 5, "Jun" },
        { 6, "Jul" },
        { 7, "Aug" },
        { 8, "Sep" },
        { 9, "Oct" },
        {10, "Nov" },
        {11, "Dec" }
};

const std::map<std::string, int> c_month_name_to_id =
{
        { "Jan", 0 },
        { "Feb", 1 },
        { "Mar", 2 },
        { "Apr", 3 },
        { "May", 4 },
        { "Jun", 5 },
        { "Jul", 6 },
        { "Aug", 7 },
        { "Sep", 8 },
        { "Oct", 9 },
        { "Nov",10 },
        { "Dec",11 }
};

const unsigned c_zero_year = 1900;


namespace {
    std::vector<std::string> TokenizeDateString(const std::string &str)
    {
        std::vector<std::string> tokens;

        bool on_token = false;

        for (const char& c : str)
        {
            bool info = isdigit(c) || isalpha(c);
            if (!on_token && info)  // new token
            {
                tokens.emplace_back();
                tokens.back().push_back(c);
                on_token = true;
            }
            else if (on_token && info)
            {
                tokens.back().push_back(c);
            }
            else if (!info)
            {
                on_token = false;
            }
        }

        return tokens;
    }


    enum class TimeChunk { DayOfWeek, Day, Month, Year, Hour, Minute, Second };

    std::vector<std::vector<TimeChunk>> c_time_standards = {
            // 1) Fri, 31 Dec 2009 23:59:59 GMT (better)
            // 2) Friday, 31-Dec-09 23:59:59 GMT
            { TimeChunk::DayOfWeek, TimeChunk::Day, TimeChunk::Month, TimeChunk::Year,
              TimeChunk::Hour, TimeChunk::Minute, TimeChunk::Second },
            // 3) Fri Dec 31 23:59:59 2009
            { TimeChunk::DayOfWeek, TimeChunk::Month, TimeChunk::Day, TimeChunk::Hour,
              TimeChunk::Minute, TimeChunk::Second, TimeChunk::Year }
    };

    int DetectTimeStandard(const std::vector<std::string>& tokens)
    {
        int res = -1;

        auto token_2nd_month_it = c_month_name_to_id.find(tokens[1]);
        if (token_2nd_month_it != c_month_name_to_id.end()) {
            res = 1;  // 3rd standard
        }
        else {
            // TODO: add additional tests
            res = 0;  // 1st or 2nd standard
        }

        return res;
    }

}


tm GMTNow()
{
    auto now = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(now);
    tm utc_tm = *gmtime(&t);
    return utc_tm;
}

tm StringToTimeT(std::string str)
{
    using namespace std;

    tm t;

    auto tokens = TokenizeDateString(str);

    if (!(tokens.size() == 8 || tokens.size() == 7))
    {
        throw TimeConverException("Bad tokens size : " + to_string(tokens.size()) + " / " + str);
    }

    // Detect time standard
    int time_standard = DetectTimeStandard(tokens);

    if (time_standard == -1)
    {
        throw TimeConverException("Bad time standard : " + str);
    }

    // Parse tokens
    for (size_t i = 0; i < c_time_standards[time_standard].size(); ++i)
    {
        switch (c_time_standards[time_standard][i])
        {
            case TimeChunk::DayOfWeek: {
                auto it = c_wday_name_to_id.find(tokens[i]);
                if (it == c_wday_name_to_id.end())
                {
                    throw TimeConverException("Bad day of week : " + tokens[i] + " / " + str);
                }
                t.tm_wday = it->second;
                break;
            }

            case TimeChunk::Day:
            {
                int day;
                try {
                    day = stoi(tokens[i]);
                }
                catch (...) {
                    throw TimeConverException("Bad day number : " + tokens[i] + " / " + str);
                }
                t.tm_mday = day;
                break;
            }

            case TimeChunk::Month:
            {
                auto it = c_month_name_to_id.find(tokens[i]);
                if (it != c_month_name_to_id.end())  // by name
                {
                    t.tm_mon = it->second;
                }
                else {  // as digit
                    int month;
                    try {
                        month = stoi(tokens[i]);
                    }
                    catch (...) {
                        throw TimeConverException("Bad month number : " + tokens[i] + " / " + str);
                    }
                    t.tm_mon = month;
                }
                break;
            }

            case TimeChunk::Year:
            {
                int year;
                try {
                    year = stoi(tokens[i]);
                }
                catch (...) {
                    throw TimeConverException("Bad year number : " + tokens[i] + " / " + str);
                }
                if (year < 100)
                    year += 2000;  // TODO: not sure about 99 -> 1999
                t.tm_year = year - c_zero_year;
                break;
            }

            case TimeChunk::Hour:
            {
                int hour;
                try {
                    hour = stoi(tokens[i]);
                }
                catch (...) {
                    throw TimeConverException("Bad hour number : " + tokens[i] + " / " + str);
                }
                if (hour < 0 || hour > 23)
                    throw TimeConverException("Bad hour : " + tokens[i] + " / " + str);

                t.tm_hour = hour;
                break;
            }

            case TimeChunk::Minute:
            {
                int min;
                try {
                    min = stoi(tokens[i]);
                }
                catch (...) {
                    throw TimeConverException("Bad minutes number : " + tokens[i] + " / " + str);
                }
                if (min < 0 || min > 59)
                    throw TimeConverException("Bad minute : " + tokens[i] + " / " + str);

                t.tm_min = min;
                break;
            }

            case TimeChunk::Second:
            {
                int sec;
                try {
                    sec = stoi(tokens[i]);
                }
                catch (...) {
                    throw TimeConverException("Bad seconds number : " + tokens[i] + " / " + str);
                }
                if (sec < 0 || sec > 59)
                    throw TimeConverException("Bad second : " + tokens[i] + " / " + str);

                t.tm_sec = sec;
                break;
            }
        }
    }

    return t; // TODO: if not GMT ?
}

std::string TimeTToString(tm utc_tm)
{
    using namespace std;

    // Format: Fri, 31 Dec 1999 23:59:59 GMT
    return c_wday_id_to_short_name.at(utc_tm.tm_wday) + ", " +
            to_string(utc_tm.tm_mday) + " " +
            c_month_id_to_name.at(utc_tm.tm_mon) + " " +
            to_string(utc_tm.tm_year + c_zero_year) + " " +
            to_string(utc_tm.tm_hour) + ":" +
            to_string(utc_tm.tm_min) + ":" +
            to_string(utc_tm.tm_sec) + " GMT";
}
