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
    
    template<typename T>
    static T strto(const std::string& str)
    {
        if (str.empty()) return T();
        std::stringstream ss(str);
        T t;
        ss >> t;
        return t;
    }
    
    template<typename T>
    static std::string tostr(const T& t)
    {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    }
    
    static std::string toupper(const std::string &s);
};

/*template<typename T>
T Common::strto(const std::string& str)

template<typename T>
std::string tostr(const T& t)*/

#endif
