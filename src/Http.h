//
//  Http.h
//
//  by jiahuan.liu
//  10/23/2016
//
#ifndef _HTTP_H
#define _HTTP_H

#include <map>
#include <vector>
#include <cassert>

#include "Common.h"
#include "Exception.h"

class Url
{
public:
    enum TYPE
    {
        HTTP,
        HTTPS
    };
    Url() :_eUrlType(HTTP) {}
    explicit Url(const std::string& sUrl);
    ~Url() {}
    std::string genUrl();
    std::string getHostPort() const;
    std::string getHost() const {return _sHost;}
    std::string getPort() const {return _sPort;}
    std::string getResourcePath() const {return _sPath;}
    std::string getRootPath() const;
    std::string getRelatPath() const;
    std::string getQuery() const;
    std::string getRef() const;
    bool parse(const std::string& sUrl);
    Url buildByRelatPath(const std::string& sRelatPath) const;
    void format();
    void clear();
protected:
    std::string _genUrl();
    std::string _getDefaultPort() const;
    bool        _isDefaultPort() const;
    static std::string _formatPath(const std::string& sPath);
protected:
    TYPE        _eUrlType;
    std::string _sDomain;   //http/https
    std::string _sHost;
    std::string _sPort;
    std::string _sPath;     //resource path
    std::string _sQuery;    //?
    std::string _sRef;      //#
    std::string _sUrl;      //parsed url
};

class HttpHeader
{
    struct CmpCase
    {
        bool operator()(const std::string &s1, const std::string &s2) const
        {
            return (strcasecmp(s1.c_str(), s2.c_str()) < 0);
        }
    };
public:
    typedef std::multimap<std::string,std::string, CmpCase> http_header_type;
public:
    struct Exception : ::Exception
    {
        Exception(const std::string& strerr) : ::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : ::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
public:
    HttpHeader() {clear(); setConnection("close");}
    //void setDefault(HttpHeader& header) {header = HttpHeader();}
public:
    std::string genHeader();
    const char* parse(const char* szBuffer);
public:
    void eraseItem(const std::string &sItem) {_header.erase(sItem);}
    void clear() {_header.clear();}
    void setCacheControl(const std::string &sCacheControl) {setItem("Cache-Control", sCacheControl);}
    void setConnection(const std::string &sConnection) {setItem("Connection", sConnection);}
    void setContentType(const std::string &sContentType) {setItem("Content-Type", sContentType);}
    void setContentLength(const size_t &iContentLength) {setItem("Content-Length", Common::tostr(iContentLength));}
    void setReferer(const std::string &sReferer) {setItem("Referer", sReferer);}
    void setHost(const std::string &sHost){setItem("Host", sHost);}
    void setAcceptEncoding(const std::string &sAcceptEncoding){setItem("Accept-Encoding", sAcceptEncoding);}
    void setAcceptLanguage(const std::string &sAcceptLanguage){setItem("Accept-Language", sAcceptLanguage);}
    void setAccept(const std::string &sAccept) {setItem("Accept", sAccept);}
    void setTransferEncoding(const std::string& sTransferEncoding) {setItem("Transfer-Encoding", sTransferEncoding);}
    std::string getHost() const {return getItem("host");}
    //size_t getHeaderLength() const {return _length;}
public:
    void setItem(const std::string &sItemName, const std::string &sItemValue);
    //for cookie fields
    void setMultivalItem(const std::string &sItemName, const std::string &sItemValue);
    std::string getItem(const std::string& sItem) const;
    std::vector<std::string> getMultivalItem(const std::string &sItemName) const;
protected:
    //size_t              _length;
    http_header_type    _header;
};

struct HttpMsg
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
    virtual size_t parseHeader(const char* sBuf) = 0;
    virtual bool decode(const std::string &sBuffer) = 0;
    virtual bool decode(const char *sBuffer, size_t iLength) = 0;
    virtual std::string encode() = 0;
    virtual void reset() {_sContent = ""; _stHeader.clear();}
public:
    HttpHeader* getHeader() {return &_stHeader;}
    std::string getContent() const {return _sContent;}
    //item in header
    size_t getContentLength() const {return Common::strto<size_t>(_stHeader.getItem("Content-Length"));}
    void setContent(const std::string &content, bool bUpdateContentLength = true);
    std::string getContentType() const {return _stHeader.getItem("Content-Type");}
protected:
    std::string _sContent;
    HttpHeader  _stHeader;
};

struct HttpRequest : public HttpMsg
{
    enum
    {
        REQUEST_GET,
        REQUEST_POST,
        REQUEST_OPTIONS,
        REQUEST_HEAD
    };
    static std::string _reqType2str(int eReqType);
public:
    struct Exception : HttpMsg::Exception
    {
        Exception(const std::string& strerr) : HttpMsg::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : HttpMsg::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
public:
    size_t parseHeader(const char* sBuf);
    bool decode(const std::string &sBuf);
    bool decode(const char *sBuf, size_t iLen);
    std::string encode();
    virtual void reset() {_eReqType = REQUEST_GET; _stUrl.clear();}
public:
    void createGetRequest(const std::string &sUrl, bool bUpdateHost = false);
    void createHeadRequest(const std::string &sUrl, bool bUpdateHost = false);
    void createPostRequest(const std::string &sUrl, const std::string &sPostBody, bool bUpdateHost = false);
    //void setPostRequest(const std::string &sUrl, const char *sBuffer, size_t iLength, bool bUpdateHost = false);
    void createOptionsRequest(const std::string &sUrl, bool bUpdateHost = false);
    bool isGET() const { return _eReqType == REQUEST_GET; }
    bool isHEAD() const { return _eReqType == REQUEST_HEAD; }
    bool isPOST() const { return _eReqType == REQUEST_POST; }
    bool isOPTIONS() const { return _eReqType == REQUEST_OPTIONS; }
    const Url &getUrl() const { return _stUrl;}
public:
    //TODO: request header
    //static bool checkRequest(const char* sBuffer, size_t len);
    void setUserAgent(const std::string &sUserAgent){_stHeader.setItem("User-Agent", sUserAgent);}
    void setCookie(const std::string &sCookie){_stHeader.setItem("Cookie", sCookie);}
    std::vector<std::string> getCookie() {return _stHeader.getMultivalItem("Cookie");}
protected:
    static std::string _getUrlRequest(const Url& stUrl);
    void _parseUrl(const std::string& sUrl);
protected:
    int _eReqType;
    Url _stUrl;
};

struct HttpResponse : public HttpMsg
{
public:
    struct Exception : HttpMsg::Exception
    {
        Exception(const std::string& strerr) : HttpMsg::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : HttpMsg::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
public:
    size_t parseHeader(const char* sBuf);
    bool decode(const std::string &sBuf);
    bool decode(const char *sBuf, size_t iLength);
    virtual bool incrementDecode(std::string &sBuf);
    std::string encode();
public:
    void createResponse(int status = 200, const std::string& about = "OK", const std::string& sBody = "");
    //void createResponse(int status, const std::string& about, const char *sBuffer, size_t iLength);
    //void setResponse(const char *sBuffer, size_t iLength);
    std::string getHeaderLine() const {return _sHeaderLine;}
    int  getStatus() const {return _iStatus; }
    void setStatus(int status) {_iStatus = status;}
    std::string getAbout() const {return _sAbout;}
    void setAbout(const std::string &about) {_sAbout = about;}
    std::string getVersion() const {return _sVersion;}
    void setVersion(const std::string &sVersion) {_sVersion = sVersion;}
public:
    //TODO: response header
    void setServer(const std::string &sServer){_stHeader.setItem("Server", sServer);}
    void setSetCookie(const std::string &sCookie){_stHeader.setItem("Set-Cookie", sCookie);}
    std::vector<std::string> getSetCookie() const {return _stHeader.getMultivalItem("Set-Cookie");}
protected:
    bool _isHeaderParsed() const {return _sHeaderLine.empty();}
protected:
    std::string _sVersion;
    int         _iStatus;
    std::string _sAbout;
    std::string _sHeaderLine;
    //size_t      _iCurContentLen;
};

class HtmlPage
{
public:
    struct Exception : ::Exception
    {
        Exception(const std::string& strerr) : ::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : ::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
};

class HttpUtil
{
public:
    static void eraseHashParam();
    static bool isValidHost();
    static std::string getLine(const char** ppChar)
    {
        std::string sTmp;
        sTmp.reserve(512);
        
        while((**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0')
        {
            sTmp.append(1, (**ppChar));
            (*ppChar)++;
        }
        if((**ppChar) == '\r')
        {
            (*ppChar)++;   //pass the char '\n'
        }
        (*ppChar)++;
        
        return sTmp;
    }
    static std::string getLine(const char** ppChar, int iBufLen)
    {
        std::string sTmp;
        sTmp.reserve(512);
        
        int iCurIndex= 0;
        while( (**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0')
        {
            if ( iCurIndex < iBufLen )
            {
                sTmp.append(1, (**ppChar));
                (*ppChar)++;
                iCurIndex++;
            }
            else
            {
                break;
            }
        }
        if( (**ppChar) == '\r')
        {
            if ( iCurIndex < iBufLen )
            {
                (*ppChar)++;   //pass the char '\n'
                iCurIndex++;
            }
        }
        if( iCurIndex < iBufLen )
        {
            (*ppChar)++;
            iCurIndex++;
        }
        
        return sTmp;
    }
};

#endif
////server
//class TcpConn
//{
//    
//};
//
//class HttpConn
//{
//    
//};