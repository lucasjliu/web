//
//  Crawler.h
//
//  by jiahuan.liu
//  10/29/2016
//

#ifndef _CRAWLER_H
#define _CRAWLER_H

#include <deque>
#include <map>
#include <set>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include "Socket.h"
#include "Http.h"
#include "Exception.h"
#include "Common.h"
#include "Logger.h"
#include "ThreadUtil.h"

class HtmlUtil
{
public:
    struct Exception : ::Exception
    {
        Exception(const std::string& strerr) : ::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : ::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
public:
    //getLine
    typedef std::string::size_type Pos;
    static Pos skipTo(const std::string& content, Pos& pos, std::string tag)
    {
        if (pos == std::string::npos) return pos;
        return (pos = content.find(tag, pos));
    }
    static Pos skip(const std::string& content, Pos& pos, std::string tag)
    {
        if (pos == std::string::npos) return pos;
        pos = content.find(tag, pos);
        if (pos != std::string::npos)
            pos += tag.length();
        return pos;
    }
    static std::string getBetween(const std::string& content, Pos& pos,
                                  const std::string& begTag, const std::string& endTag)
    {
        if (pos == std::string::npos) return "";
        skip(content, pos, begTag);
        std::string sRet;
        if (pos != std::string::npos)
        {
            Pos pos1 = content.find(endTag, pos);
            if (pos1 != std::string::npos)
            {
                sRet = content.substr(pos, pos1 - pos);
                pos1 += endTag.length();
            }
            pos = pos1;
        }
        return sRet;
    }
    static void filter(std::string& sUrl)
    {
        std::stringstream ss(sUrl);
        std::string sTmp;
        sUrl.clear();
        while (ss >> sTmp)
        {
            sUrl += sTmp;
        }
    }
    static void preClean(std::string& content, std::set<std::string> tags)
    {
        
    }
};

class MyCrawler
{
public:
    struct Article
    {
        std::string sTitle;
        std::string sBody;
        
        std::string genContent() const
        {
            return sTitle + "|" + sBody;
        }
    };
public:
    MyCrawler()
    {
        reset();
    }
    ~MyCrawler() {stop();}
    void init(std::string sHostUrl,
              int iArticleTrdNum = 5,
              int iArchiveTrdNum = 2,
              int iListTrdNum = 2,
              int iMaxDoc = 100,
              std::string sWorkPlace = "./",
              std::set<std::string> targetArchive =
              {
                  "/news/archive/worldNews",
                  "/news/archive/businessNews",
                  "/news/archive/technologyNews"
              })
    {
        stop();
        reset();
        
        _hostUrl.parse(sHostUrl);
        _archiveTasks.push(_hostUrl);
        
        _iArticleTrdNum = iArticleTrdNum;
        _iArchiveTrdNum = iArchiveTrdNum;
        _iListTrdNum = iListTrdNum;
        _iMaxDocNum = iMaxDoc;
        _sWorkPlace = sWorkPlace;
        
        _targetArchives = targetArchive;
    }
    void run();
    void stop() {}
    void reset() {}
    void crawlArticle();
    void crawlList();
    void crawlArchive();
    bool fetch(Url& url, std::string& sContent);
    void parseArticle(const std::string& sContent, Article& stArticle);
    void parseList(const std::string& sContent, std::vector<Url>& vUrls);
    void parseArchive(const std::string& sContent, std::vector<Url>& vUrls, std::vector<Url>& vArchives);
    void log(std::string sMsg);
    void addArchive(Url url);
private:
    static std::string _genDocName(const int& docid)
    {
        char sBuf[10] = {0};
        sprintf(sBuf, "%09d", docid);
        return std::string(sBuf);
    }
    static std::string _genPageUrl(const Url& archiveUrl, const int& pageNum)
    {
        return archiveUrl.getHost() + archiveUrl.getResourcePath() +
            "?view=page&page=" + Common::tostr(pageNum) + "&pageSize=10";
    }
    int _doRequest(Url& url, HttpResponse& stRsp, int iTimeout = 3000);
    int _doRequest(const std::string& sUrl, HttpResponse& stRsp, int iTimeout = 3000);
    bool _isEnd() {return _docidSvr.get() >= _iMaxDocNum;}
protected:
    Deque<Url>      _articleTasks;
    Semaphore       _articleSem;
    Deque<Url>      _archiveTasks;
    Semaphore       _archiveSem;
    Deque<Url>      _listTasks;
    Semaphore       _listSem;
    
    IdServer        _docidSvr;
    Md5Server       _archiveMd5;
    Md5Server       _articleMd5;
    
    std::mutex      _mtxLog;
    
    int             _iArticleTrdNum;
    int             _iArchiveTrdNum;
    int             _iListTrdNum;
    int             _iMaxDocNum;
    
    Url             _hostUrl;
    
    std::string     _sWorkPlace;
    
    std::vector<std::thread> _trdList;
    
    std::set<std::string> _targetArchives;
};

#endif//_CRAWLER_H
