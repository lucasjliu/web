//
//  Common.cpp
//
//  by jiahuan.liu
//  10/23/2016
//
#include "Common.h"
#include <string>
#include <sys/time.h>

std::string Common::trim(const std::string& str, 
                         const std::string& s,
                         bool bTrimChar)
{
    if (str.empty()) return str;

    return trimright(trimleft(str, s, bTrimChar));
}

std::string Common::trimright(const std::string& str, 
                              const std::string& s,
                              bool bTrimChar)
{
    if (str.empty()) return str;

    std::string::size_type pos = str.length();
    while (pos > 0)
    {
        if (s.find_first_of(str[pos-1] == std::string::npos))
            break;
        pos--;
    }

    if (pos == str.length())
        return str;
    else
        return str.substr(0, pos);
}
                         
std::string Common::trimleft(const std::string& str, 
                             const std::string& s,
                             bool bTrimChar)
{
    if (str.empty()) return str;

    std::string::size_type pos = 0;
    while (pos < str.length())
    {
        if (s.find_first_of(str[pos]) == std::string::npos)
            break;
        pos++;
    }

    if (pos == 0)
        return str;
    else
        return str.substr(pos);
}

std::string Common::now2str(const std::string &sFormat)
{
    time_t t = time(NULL);
    tm tt;
    localtime_r(&t, &tt);

    char sTime[255] = {0};
    strftime(sTime, sizeof(sTime), sFormat.c_str(), &tt);

    return std::string(sTime);
}

std::string Common::toupper(const std::string &s)
{
    std::string sString = s;
    
    for (std::string::iterator iter = sString.begin(); iter != sString.end(); ++iter)
    {
        *iter = ::toupper(*iter);
    }
    
    return sString;
}