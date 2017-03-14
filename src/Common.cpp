//
//  Common.cpp
//
//  by jiahuan.liu
//  10/23/2016
//
#include "Common.h"
#include <string>
#include <sys/time.h>
#include <utility>

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
    std::string str = s;
    
    for (char& ch : str)
    {
        ch = ::toupper(ch);
    }
    
    return std::move(str);
}

void Common::toupper(std::string &s)
{
    for (char& ch : s)
    {
        ch = ::toupper(ch);
    }
}

std::string Common::tolower(const std::string &s)
{
    std::string str = s;
    
    for (char& ch : str)
    {
        ch = ::tolower(ch);
    }
    
    return std::move(str);
}

void Common::tolower(std::string &s)
{
    for (char& ch : s)
    {
        ch = ::tolower(ch);
    }
}

std::string Common::replace(const std::string &sString, const std::string &sSrc, const std::string &sDest)
{
    if(sSrc.empty())
    {
        return sString;
    }
    
    std::string sBuf = sString;
    
    std::string::size_type pos = 0;
    
    while( (pos = sBuf.find(sSrc, pos)) != std::string::npos)
    {
        sBuf.replace(pos, sSrc.length(), sDest);
        pos += sDest.length();
    }
    
    return std::move(sBuf);
}

void Common::replace(std::string &sBuf, const std::string &sSrc, const std::string &sDest)
{
    if(sSrc.empty())
    {
        return;
    }
    
    std::string::size_type pos = 0;
    
    while( (pos = sBuf.find(sSrc, pos)) != std::string::npos)
    {
        sBuf.replace(pos, sSrc.length(), sDest);
        pos += sDest.length();
    }
}

bool Common::divstr(const std::string& sStr,
                    const std::string &sSep,
                    std::string& sDest1,
                    std::string& sDest2)
{
    std::string::size_type pos = sStr.find_first_of(sSep);
    
    if (pos == std::string::npos)
    {
        return false;
    }
    
    sDest1 = sStr.substr(0, pos);
    sDest2 = sStr.substr(pos + 1);
    
    return true;
}

template<>
std::vector<std::string> Common::sepstr(const std::string &sStr,
                                        const std::string &sSep,
                                        bool withEmpty)
{
    std::vector<std::string> vt;
    
    std::string::size_type pos = 0;
    std::string::size_type pos1 = 0;
    
    while(true)
    {
        std::string s;
        pos1 = sStr.find_first_of(sSep, pos);
        if(pos1 == std::string::npos)
        {
            if(pos + 1 <= sStr.length())
            {
                s = sStr.substr(pos);
            }
        }
        else if(pos1 == pos)
        {
            s = "";
        }
        else
        {
            s = sStr.substr(pos, pos1 - pos);
            pos = pos1;
        }
        
        if(withEmpty)
        {
            vt.push_back((std::move(s)));
        }
        else
        {
            if(!s.empty())
            {
                vt.push_back(std::move(s));
            }
        }
        
        if(pos1 == std::string::npos)
        {
            break;
        }
        
        pos++;
    }
    
    return std::move(vt);
}