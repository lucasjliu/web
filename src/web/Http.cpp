//
//  Http.cpp
//
//  by jiahuan.liu
//  10/23/2016
//
#include "Http.h"
#include "Common.h"

Url::Url(const std::string& sUrl)
{
    parse(sUrl);
}

std::string Url::genUrl()
{
    if (_sUrl.empty())
        _genUrl();
    return _sUrl;
}

std::string Url::_genUrl()
{
    _sUrl.clear();

    _sUrl = _sDomain + "://" + _sHost;
    
    if (!_isDefaultPort())
        _sUrl += ":" + _sPort;
    
    _sUrl += _sPath;

    if (!_sQuery.empty())
        _sUrl += "?" + _sQuery;

    if (!_sRef.empty())
        _sUrl += "#" + _sRef;

    return _sUrl;
}

std::string Url::getHostPort() const
{
   if (!_isDefaultPort())
       return _sHost + ":" + _sPort;
    return _sHost;
}

std::string Url::getRootPath() const
{
    return _sDomain + "://" + getHostPort() + "/";
}

std::string Url::getRelatPath() const
{
    std::string sPath = _sPath;
    std::string::size_type pos = _sPath.rfind("/");

    if (pos == std::string::npos)
        return "/";
    else
        return sPath.substr(0, pos + 1);
}

std::string Url::getQuery() const
{
    return _sQuery;
}

std::string Url::getRef() const
{
    return _sRef;
}

std::string Url::_getDefaultPort() const
{
    switch(_eUrlType)
    {
    case HTTP:
        return "80";
    case HTTPS:
        return "443";
    }
    return "80";
}

bool Url::_isDefaultPort() const
{
    switch(_eUrlType)
    {
    case HTTP:
        return _sPort == "80";
    case HTTPS:
        return _sPort == "443";
    }
    return false;
}

bool Url::parse(const std::string& sUrl)
{
    std::string url = Common::trim(sUrl, " ");
    
    if (url.empty())
    {
        return false;
    }

    clear();

    int iPos = 0;
    
    if (strncasecmp(url.c_str(), "http://", 7) == 0)
    {
        _eUrlType = HTTP;
        _sDomain = "http";
        iPos = 7;
    }
    else if (strncasecmp(url.c_str(), "https://", 8) == 0)
    {
        _eUrlType = HTTPS;
        _sDomain = "https";
        iPos = 8;
    }
    else if (url.find("://") == std::string::npos)
    {
        _eUrlType = HTTP;
        _sDomain = "http";
    }
    else//protocol not supported
    {
        return false;
    }
    
    std::string::size_type index, qIndex, nIndex;
    index = url.find("/", iPos);
    qIndex = url.find("?", iPos);
    nIndex = url.find("#", iPos);
    
    if (index != std::string::npos &&
        ((index > qIndex && qIndex != std::string::npos)
         || (index > nIndex && nIndex != std::string::npos)))
        return false;
    
    std::string sPath;
    if (index == std::string::npos)
    {
        sPath = url.substr(iPos);
        _sHost = "";
    }
    else
    {
        sPath = url.substr(index);
        _sHost = url.substr(iPos, index - iPos);
    }

    index = _sHost.find(":");
    if (index == std::string::npos)
    {
        _sPort = _getDefaultPort();
    }
    else
    {
        _sPort = _sHost.substr(index + 1);
        _sHost = _sHost.substr(0, index);
    }
        
    index = sPath.find("?");
    if(index != std::string::npos)
    {
        _sPath = sPath.substr(0, index);
        _sQuery = sPath.substr(index + 1);
        
        index = _sQuery.rfind("#");
        if(index != std::string::npos)
        {
            _sRef = _sQuery.substr(index + 1);
            _sQuery = _sQuery.substr(0, index);
        }
    }
    else
    {
        _sPath = sPath;
        _sQuery = "";
        
        index = _sPath.rfind("#");
        if(index != std::string::npos)
        {
            _sRef = _sPath.substr(index + 1);
            _sPath = _sPath.substr(0, index);
        }
    }
    
    if (_sPath.empty())
    {
        _sPath = "/";
    }
    
    _genUrl();
    
    return (_sHost.find(".") != std::string::npos);
}
    
Url Url::buildByRelatPath(const std::string& sRelatPath) const
{
    std::string sUrl;
    
    if(!sRelatPath.empty() && sRelatPath[0] == '/')
    {
        sUrl = sRelatPath.substr(1); //pad with host path if begin with '/'
    }
    else if(sRelatPath[0] == '#')
    {
        //#
        sUrl = _sPath.substr(1);
        
        if(!_sQuery.empty())
            sUrl += "?" + _sQuery;
        
        sUrl += sRelatPath;
    }
    else
    {
        //current relative path
        sUrl = getRelatPath().substr(1) + sRelatPath;
    }
    
    sUrl = getRootPath() + _formatPath("/" + sUrl).substr(1);
    
    Url url;
    url.parse(sUrl);
    
    return url;
}

void Url::format()
{
    _sPath = _formatPath(_sPath);
    _genUrl();
}

std::string Url::_formatPath(const std::string& sPath)
{
    size_t pos           = 0;
    std::string sNewPath = sPath;
    
    while (true)
    {
        size_t dotPos = sNewPath.find("./", pos);
        if (dotPos != std::string::npos)
        {
            if ((dotPos == 0) || (sNewPath.at(dotPos - 1) == '/'))
            {
                sNewPath.erase(dotPos, 2);
            }
            else
            {
                pos = dotPos + 2;
            }
        }
        else
        {
            break;
        }
    }
    
    //erase ending .
    if (((sNewPath.length() >= 2) && (sNewPath.substr(sNewPath.length()-2) == "/.")) || (sNewPath == "."))
    {
        sNewPath.erase(sNewPath.length() - 1);
    }
    
    //handle /../
    pos = 0;
    size_t startPos = 0;
    
    while (true)
    {
        size_t slashDot = sNewPath.find("/../", pos);
        if (slashDot != std::string::npos)
        {
            if (0 == slashDot)
            {
                sNewPath.erase(0, 3);
                continue;
            }
            if ( (slashDot > 1) && (sNewPath.substr(slashDot - 2, 2) == "..") )
            {
                pos = slashDot + 4;
                continue;
            }
            
            startPos = sNewPath.rfind('/', slashDot - 1);
            if (startPos == std::string::npos) startPos = 0;
            
            sNewPath.erase(startPos, slashDot + 4 - startPos - 1);
        }
        else
        {
            break;
        }
    }
    
    //handle ending with /..
    if ((sNewPath.size() >= 3) && (sNewPath.substr(sNewPath.size() - 3, 3) == "/.."))
    {
        size_t slashDot = sNewPath.size() - 3;
        if (!((slashDot > 1) && (sNewPath.substr(slashDot - 2, 2) == "..")))
        {
            startPos = sNewPath.rfind ('/', slashDot - 1);
            if (startPos == std::string::npos) startPos = 0;
            sNewPath.erase (startPos + 1);
        }
    }
    
    return sNewPath;
}

void Url::clear()
{
    _sDomain = "";
    _sHost = "";
    _sPort = "";
    _sPath = "";
    _sQuery = "";
    _sRef = "";
    _sUrl = "";
}

std::string HttpHeader::genHeader()
{
    std::string sHeader;
    
    for (http_header_type::const_iterator it = _header.begin();
         it != _header.end();
         ++it)
    {
        if (it->second != "")
        {
            sHeader = sHeader + it->first + ": " + it->second + "\r\n";
        }
    }
    
    return sHeader;
}

const char* HttpHeader::parse(const char* sBuf)
{
    _header.clear();
    
    const char** ppChar = &sBuf;
    long pStart = (long)*ppChar;
    long length = strlen(sBuf);
    
    while ((long)(*ppChar - pStart) < length)
    {
        std::string sLine = HttpUtil::getLine(ppChar);
        
        if (sLine.empty()) break;

        if(strncasecmp(sLine.c_str(), "GET ", 4) ==0
           || strncasecmp(sLine.c_str(), "POST ", 5) ==0
           || strncasecmp(sLine.c_str(), "OPTIONS ", 8) ==0
           || strncasecmp(sLine.c_str(), "HEAD ", 5) ==0
           || strncasecmp(sLine.c_str(), "HTTP/", 5) ==0)
        {
            continue;
        }
        
        std::string::size_type index = sLine.find(":");
        if (index != std::string::npos)
        {
            _header.insert(http_header_type::value_type(
                Common::trim(sLine.substr(0, index), " "),
                Common::trim(sLine.substr(index), " "))
            );
        }
    }
   
    return *ppChar;
}

void HttpHeader::setItem(const std::string &sItemName, const std::string &sItemValue)
{
    const char * pStr = "SET-COOKIE";
    //const char * pStr2 = "COOKIE";
    if((strcasecmp(sItemName.c_str(), pStr) != 0))// && (strcasecmp(sItemName.c_str(), pStr2) != 0))
        _header.erase(sItemName);
    _header.insert(std::multimap<std::string, std::string>::value_type(sItemName, sItemValue));
}

void HttpHeader::setMultivalItem(const std::string &sItemName, const std::string &sItemValue)
{
    _header.insert(std::multimap<std::string, std::string>::value_type(sItemName, sItemValue));
}

std::string HttpHeader::getItem(const std::string& sItem) const
{
    http_header_type::const_iterator it;
    if((it = _header.find(sItem)) == _header.end())
        return "";
    return it->second;
}

std::vector<std::string> HttpHeader::getMultivalItem(const std::string &sItemName) const
{
    std::vector<std::string> v;
    for(http_header_type::const_iterator it = _header.begin();
        it != _header.end();
        ++it)
    {
        if(strcasecmp(it->first.c_str(), sItemName.c_str()) == 0)
        {
            v.push_back(it->second);
        }
    }
    return v;
}


void HttpMsg::setContent(const std::string &content, bool bUpdateContentLength)
{
    _sContent = content;
    
    if(bUpdateContentLength)
    {
        _stHeader.eraseItem("Content-Length");
        if(_sContent.length() > 0)
            _stHeader.setContentLength(_sContent.length());
    }
}

std::string HttpRequest::_reqType2str(int eReqType)
{
    if(eReqType == REQUEST_GET)
        return "GET";
    else if(eReqType == REQUEST_HEAD)
        return "HEAD";
    else if(eReqType == REQUEST_POST)
        return "POST";
    else if(eReqType == REQUEST_OPTIONS)
        return "OPTIONS";
    return "";
}

size_t HttpRequest::parseHeader(const char* sBuf)
{
    const char *pStart = sBuf;
    const char **ppChar = &sBuf;
    
    std::string sLine = HttpUtil::getLine(ppChar);
    std::string::size_type pos = sLine.find(" ");
    if(pos == std::string::npos)
    {
        throw Exception("[HttpRequest::parseHeader] http format error: " + sLine);
    }
    
    std::string sMethod = Common::trim(sLine.substr(0, pos));
    
    if(strncasecmp(sMethod.c_str(), "GET", 3) ==0)
        _eReqType = REQUEST_GET;
    else if(strncasecmp(sMethod.c_str(), "POST", 4) ==0)
        _eReqType = REQUEST_POST;
    else if(strncasecmp(sMethod.c_str(), "OPTIONS", 7) ==0)
        _eReqType = REQUEST_OPTIONS;
    else if(strncasecmp(sMethod.c_str(), "HEAD", 4) == 0)
        _eReqType = REQUEST_HEAD;
    else
    {
        throw Exception("[HttpRequest::parseHeader] request type error: " + sLine);
    }
    
    std::string::size_type pos1 = sLine.rfind(" ");
    if(pos1 == std::string::npos || pos1 <= pos)
    {
        throw Exception("[HttpRequest::parseHeader] http format error: " + sLine);
    }
    
    //std::string sVersion = Common::toupper(Common::trim(sLine.substr(pos1+1)));
    
    std::string sUrl = Common::trim(sLine.substr(pos+1, pos1 - pos));
    if(strncasecmp(sUrl.c_str(), "https://", 8) !=0 )
    {
        if(strncasecmp(sUrl.c_str(), "http://", 7) !=0 )
        {
            sUrl = "http://" + _stHeader.getItem("Host") + sUrl;
        }
    }
    _parseUrl(sUrl);
    
    size_t n = _stHeader.parse(*ppChar) - pStart;
    
    return n;
}

bool HttpRequest::decode(const std::string& sBuf)
{
    return decode(sBuf.c_str(), sBuf.length());
}

bool HttpRequest::decode(const char* sBuf, size_t iLen)
{
    assert(sBuf);
    
    if(strncasecmp(sBuf, "GET " ,4) !=0 && strncasecmp(sBuf, "POST " ,5) !=0 &&
       strncasecmp(sBuf, "OPTIONS " ,8) !=0 && strncasecmp(sBuf, "HEAD " ,5))
    {
        throw Exception("[HttpRequest::decode] method not supported.");
    }
    
    if(strstr(sBuf, "\r\n\r\n") == NULL)
    {
        return false;
    }
    
    size_t iHeaderLen = parseHeader(sBuf);
    
    if (iLen < iHeaderLen)
        return false;
    
    _sContent.assign(sBuf + iHeaderLen, iLen - iHeaderLen);
    
    return (getContentLength() == _sContent.length());
}

std::string HttpRequest::encode()
{
    std::string sRet;
    
    if (_eReqType == REQUEST_POST)
    {
        _stHeader.setContentLength(_sContent.length());
    }
    
    sRet += _reqType2str(_eReqType) + " ";
    
    sRet += _getUrlRequest(_stUrl) + " ";
    
    sRet += "HTTP/1.1\r\n";
    
    sRet += _stHeader.genHeader() + "\r\n";
    
    if (_eReqType == REQUEST_POST)
    {
        sRet += _sContent;
    }
    
    return sRet;
}

std::string HttpRequest::_getUrlRequest(const Url& stUrl)
{
    std::string sRet = stUrl.getResourcePath();
    
    std::string sQuery = stUrl.getQuery();
    if (!sQuery.empty())
    {
        sRet += "?" + sQuery;
    }
    //without hash parameter.
    return sRet;
}

void HttpRequest::createGetRequest(const std::string &sUrl, bool bUpdateHost)
{
    if(bUpdateHost)
    {
        _stHeader.eraseItem("Host");
    }
    
    _parseUrl(sUrl);
    
    _eReqType = REQUEST_GET;
    
    _sContent = "";
    
    _stHeader.eraseItem("Content-Length");
}

void HttpRequest::createHeadRequest(const std::string &sUrl, bool bUpdateHost)
{
    if(bUpdateHost)
    {
        _stHeader.eraseItem("Host");
    }
    
    _parseUrl(sUrl);
    
    _eReqType = REQUEST_HEAD;
    
    _sContent = "";
    
    _stHeader.eraseItem("Content-Length");
}

void HttpRequest::createPostRequest(const std::string &sUrl, const std::string &sPostBody,
                    bool bUpdateHost)
{
    if(bUpdateHost)
    {
        _stHeader.eraseItem("Host");
    }
    
    _parseUrl(sUrl);
    
    _eReqType = REQUEST_POST;
    
    _sContent = sPostBody;
    
    _stHeader.setItem("Content-Length", Common::tostr(_sContent.length()));
}

void HttpRequest::createOptionsRequest(const std::string &sUrl, bool bUpdateHost)
{
    if(bUpdateHost)
    {
        _stHeader.eraseItem("Host");
    }
    
    _parseUrl(sUrl);
    
    _eReqType = REQUEST_OPTIONS;
    
    _sContent = "";
    
    _stHeader.eraseItem("Content-Length");
}

void HttpRequest::_parseUrl(const std::string& sUrl)
{
    _stUrl.parse(sUrl);
    
    if (_stHeader.getItem("Host").empty())
    {
        _stHeader.setHost(_stUrl.getHostPort());
    }
}

size_t HttpResponse::parseHeader(const char* sBuf)
{
    const char** ppChar = &sBuf;
    const char* pStart = sBuf;
    
    _sHeaderLine = Common::trim(HttpUtil::getLine(ppChar));
    std::string sLine =  _sHeaderLine;
    
    std::string::size_type pos = _sHeaderLine.find(" ");
    if (pos != std::string::npos)
    {
        _sVersion = _sHeaderLine.substr(0, pos);
        sLine = sLine.substr(pos + 1);
        
        std::string::size_type pos1 = sLine.find(" ");
        if (pos1 != std::string::npos)
        {
            _iStatus = Common::strto<int>(sLine.substr(0, pos1));
            _sAbout = Common::trim(sLine.substr(pos1));
        }
        
        size_t n = _stHeader.parse(*ppChar) - pStart;
        return n;
    }
    else
    {
        _sVersion = _sHeaderLine;
        _iStatus = 0;
        _sAbout = "";
        return _sHeaderLine.length();
    }
}

bool HttpResponse::decode(const std::string &sBuff)
{
    if (sBuff.find("\r\n\r\n") == std::string::npos)
    {
        return false;
    }
    
    size_t iHeaderLen = parseHeader(sBuff.c_str());
    
    ////////////////////////////////////to be removed
    assert(iHeaderLen == sBuff.find("\r\n\r\n") + 4);
    
    std::string sBuf = sBuff.substr(iHeaderLen);
    _sContent += sBuf;
    sBuf = "";
    _stHeader.setContentLength(getContent().length());
    
    std::string sContentLen = _stHeader.getItem("Content-Length");
    if (_sContent.empty() || _sContent == "0" || _iStatus == 204 || _iStatus == 304 ||
        ((_iStatus == 301 || _iStatus == 302) && _stHeader.getItem("Location").empty()))
        throw Exception("[HttpResponse::decode] incremental decode not supported");
    size_t iContentLen = Common::strto<size_t>(sContentLen);
    return iContentLen >= sBuff.length() - iHeaderLen;
}

bool HttpResponse::decode(const char *sBuf, size_t iLen)
{
    std::string tmp(sBuf, iLen);
    return decode(tmp);
}

bool HttpResponse::incrementDecode(std::string& sBuf)
{
    if (!_isHeaderParsed())
    {
        if (sBuf.find("\r\n\r\n") == std::string::npos)
        {
            return false;
        }
        
        size_t iHeaderLen = parseHeader(sBuf.c_str());
        
        if (_iStatus == 204)
        {//should wait for further content
            return true;
        }
        ///////////////////////////////////
        assert(iHeaderLen == sBuf.find("\r\n\r\n") + 4);
        
        sBuf = sBuf.substr(iHeaderLen);
        
        //TODO: 301, 302
        //TODO: chunk
        
        std::string sContentLen = _stHeader.getItem("Content-Length");
        if (sContentLen.empty())
        {
            if (_iStatus == 304)
            {
                return true;
            }
            
            _sContent += sBuf;
            sBuf = "";
            _stHeader.setContentLength(getContent().length());
            return false;
        }
        else
        {
            _sContent += sBuf;
            sBuf = "";
            _stHeader.setContentLength(getContent().length());
            return true;
        }
    }
    return false;
}

std::string HttpResponse::encode()
{
    std::string sRet;
    sRet += _sVersion + " ";
    sRet += Common::tostr(_iStatus) + " ";
    sRet += _sAbout + "\r\n";
    sRet += _stHeader.genHeader() + "\r\n";
    sRet += getContent();
    return sRet;
}

void HttpResponse::createResponse(int status, const std::string& about, const std::string& sBody)
{
    _iStatus = status;
    _sAbout = about;
    _sContent = sBody;
    
    _sHeaderLine = _sVersion + " " + Common::tostr(_iStatus) + " " + _sAbout;
    
    _stHeader.setContentLength(_sContent.length());
}
