//
//  Common.h
//
//  by jiahuan.liu
//  10/23/2016
//
#ifndef _COMMON_H
#define _COMMON_H

#include <string>
#include <sstream>
#include <vector>
#include <utility>

class Common
{
public:
    static std::string trim(const std::string& str, 
                            const std::string& s = " \r\n\t",
                            bool bTrimChar = true);
    
    static std::string trimright(const std::string& str, 
                                 const std::string& s = " \r\n\t",
                                 bool bTrimChar = true);
    
    static std::string trimleft(const std::string& str, 
                                const std::string& s = " \r\n\t",
                                bool bTrimChar = true);
    
    static std::string now2str(const std::string &sFormat = "%Y%m%d%H%M%S");
    
    static std::string replace(const std::string &sString,
                               const std::string &sSrc,
                               const std::string &sDest);
    
    static void replace(std::string &sString,
                        const std::string &sSrc,
                        const std::string &sDest);
    
    template<typename T>
    static T strto(const std::string& str);
    
    template<typename T>
    static T strto(std::string&& str);
    
    template<typename T>
    static std::string tostr(const T& t);
    
    static std::string toupper(const std::string &s);
    
    static void toupper(std::string &s);
    
    static std::string tolower(const std::string &s);
    
    static void tolower(std::string &s);
    
    template<typename T>
    static std::vector<T> sepstr(const std::string &sStr,
                                 const std::string &sSep,
                                 bool withEmpty = true);
    
    static bool divstr(const std::string& sStr,
                       const std::string &sSep,
                       std::string& sDest1,
                       std::string& sDest2);
};

template<typename T>
T Common::strto(const std::string& str)
{
    if (str.empty()) return T();
    std::stringstream ss(str);
    T t;
    ss >> t;
    return t;
}

template<typename T>
T Common::strto(std::string&& str)
{
    if (str.empty()) return T();
    std::stringstream ss(std::forward<std::string>(str));
    T t;
    ss >> t;
    return t;
}

template<typename T>
std::string Common::tostr(const T& t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

template<>
std::vector<std::string> Common::sepstr(const std::string &sStr,
                                        const std::string &sSep,
                                        bool withEmpty);

template<typename T>
std::vector<T> Common::sepstr(const std::string &sStr, const std::string &sSep, bool withEmpty)
{
    std::vector<T> vt;
    
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
            vt.push_back(strto<T>(std::move(s)));
        }
        else
        {
            if(!s.empty())
            {
                T tmp = strto<T>(std::move(s));
                vt.push_back(tmp);
            }
        }
        
        if(pos1 == std::string::npos)
        {
            break;
        }
        
        pos++;
    }
    
    return vt;
}

class Noncopyable
{
protected:
    Noncopyable() = default;
    virtual ~Noncopyable() {}
    
    Noncopyable(const Noncopyable& ) = delete;
    Noncopyable& operator = (const Noncopyable) = delete;
};

#endif
