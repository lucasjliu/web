//
//  Socket.cpp
//
//  by jiahuan.liu
//  10/13/2016
//

#include "Socket.h"
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>

Socket::Socket() : _sockfd(INVALID_SOCKET), _isOwner(true), _iFamily(AF_INET)
{
}

Socket::~Socket()
{
    if (_isOwner)
    {
        close();
    }
}

void Socket::init(int fd, bool isOwner, int iFamily)
{
    if (_isOwner)
    {
        close();
    }
    _sockfd = fd;
    _isOwner = isOwner;
    _iFamily = iFamily;
}

void Socket::close()
{
    if (_sockfd != INVALID_SOCKET)
    {
        if(::close(_sockfd) < 0)
        {
            //log
        }
        _sockfd = INVALID_SOCKET;
    }
}

void Socket::shutdown(int iHow)
{
    if (::shutdown(_sockfd, iHow) < 0)
    {
        //throw
    }
}

void Socket::create(int iSocketType, int iFamily)
{
    assert(iSocketType == SOCK_STREAM || iSocketType == SOCK_DGRAM);
    close();
    
    _iFamily = iFamily;
    _sockfd = socket(iFamily, iSocketType, 0);
    
    if (_sockfd < 0)
    {
        _sockfd = INVALID_SOCKET;
        //throw create socket error
    }
}

void Socket::setOwner(bool isOwner)
{
    _isOwner = isOwner;
}

void Socket::setFamily(int iFamily)
{
    _iFamily = iFamily;
}

int Socket::getfd()
{
    return _sockfd;
}

bool Socket::isValid()
{
    return _sockfd != INVALID_SOCKET;
}

void Socket::connect(const std::string& sServerAddr, uint16_t uPort)
{
    if (_iFamily != AF_INET)
    {
        //abort or throw family conflict error
    }
    else if (_connect(sServerAddr, uPort) < 0)
    {
        throw ConnectException("[Socket::connect] error", errno);
    }
}

void Socket::connect(const char* sLocalPath)
{//for AF_UNIX/AF_LOCAL
    if (_iFamily != AF_UNIX)
    {
        //abort or throw family conflict error
    }
    else if (_bind(sLocalPath) < 0)
    {
        throw ConnectException("[Socket::connect] error", errno);
    }
}

void Socket::bind(sockaddr* stLocalAddr)
{
    if (_iFamily != stLocalAddr->sa_family)
    {
        //abort or throw address family conflict error
    }
    else if (_bind(stLocalAddr, sizeof(sockaddr)) < 0)
    {
        //throw bind saddr:port error;
        throw Exception("[Socket::bind] error", errno);
    }
}

void Socket::bind(const uint16_t& uLocalPort, const std::string& sBindAddr)
{
    if (_iFamily != AF_INET)
    {
        //abort or throw address family conflict error
    }
    else if (_bind(uLocalPort, sBindAddr) < 0)
    {
        //throw bind saddr:port error;
        throw Exception("[Socket::bind] error", errno);
    }
}

void Socket::bind(const char* sUnixPath)
{ //for AF_UNIX/AF_LOCAL
    if (_iFamily != AF_UNIX)
    {
        //abort or throw address family conflict error
    }
    else if (_connect(sUnixPath) < 0)
    {
        //throw bind saddr:port error;
        throw Exception("[Socket::bind] error: ", errno);
    }
}

void Socket::listen(int iBackLog)
{
    if (::listen(_sockfd, iBackLog) < 0)
    {
        //throw listen error
        throw Exception("[Socket::listen] error: ", errno);
    }
}

int Socket::accept(Socket& remoteSockfd, sockaddr* stRemoteAddr, socklen_t& iSocklen)
{
    assert(remoteSockfd._sockfd == -1);
    
    int iSock;
    while((iSock = ::accept(_sockfd, stRemoteAddr, &iSocklen)) < 0
          &&  errno == EINTR);
    
    remoteSockfd._sockfd = iSock;
    remoteSockfd._iFamily = _iFamily;
    
    return remoteSockfd._sockfd;
}

void Socket::getSockAddr(const std::string& sSockAddr, const uint16_t& uPort,
                         sockaddr& stSockAddr) const
{
    bzero(&stSockAddr, sizeof(sockaddr));
    
    sockaddr_in *p = (sockaddr_in*)&stSockAddr;
    p->sin_family = _iFamily;
    p->sin_port = htons(uPort);
    
    if (sSockAddr != "")
    {
        sockaddr_in *p = (sockaddr_in*)&stSockAddr;
        _parseAddr(sSockAddr, p->sin_addr);
    }
}

void Socket::getUnixAddr(const char* sUnixPath, sockaddr& stUnixAddr) const
{
    bzero(&stUnixAddr, sizeof(sockaddr));
    
    sockaddr_un *p = (sockaddr_un*)&stUnixAddr;
    p->sun_family = _iFamily;
    strncpy(p->sun_path, sUnixPath, sizeof(p->sun_path));
}

void Socket::_getHostByName(const std::string& sAddr, in_addr& stInAddr) const
{
    int iErr;
    addrinfo hints, *res;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = _iFamily;
    hints.ai_flags = AI_PASSIVE;
    
    iErr = getaddrinfo(sAddr.c_str(), "", &hints, &res);
    if (iErr == 0)
    {//may have more than one result
        stInAddr = ((sockaddr_in *)res->ai_addr)->sin_addr;
    }
    else
    {
        throw Exception("Socket::getAddrInfo error", iErr);
    }
}

void Socket::_parseAddr(const std::string& sAddr, in_addr& stInAddr) const
{
    int iRet = inet_pton(_iFamily, sAddr.c_str(), &stInAddr);
    if (iRet < 0)
    {
        //throw
    }
    else if (iRet == 0)
    {
        _getHostByName(sAddr, stInAddr);
    }
}

int Socket::_connect(const std::string& sServerAddr, uint16_t uPort) noexcept
{
    sockaddr stServerAddr;
    this->getSockAddr(sServerAddr, uPort, stServerAddr);
    return _connect(&stServerAddr, sizeof(sockaddr));
}

int Socket::_connect(const char* sUnixPath) noexcept
{
    sockaddr stServerAddr;
    this->getUnixAddr(sUnixPath, stServerAddr);
    return _connect(&stServerAddr, sizeof(sockaddr));
}

int Socket::_connect(sockaddr* pstServerAddr, socklen_t addrlen) noexcept
{
    return ::connect(_sockfd, pstServerAddr, addrlen);
}

int Socket::_bind(uint16_t uLocalPort, const std::string& sLocalAddr) noexcept
{
    sockaddr stLocalAddr;
    sockaddr_in *p = (sockaddr_in*)&stLocalAddr;
    this->getSockAddr(sLocalAddr, uLocalPort, stLocalAddr);
    if (sLocalAddr == "")
    {
        p->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    return _bind(&stLocalAddr, sizeof(sockaddr));
}

int Socket::_bind(const char* sUnixPath) noexcept
{
    sockaddr stBindAddr;
    this->getUnixAddr(sUnixPath, stBindAddr);
    return _bind(&stBindAddr, sizeof(sockaddr));
}

int Socket::_bind(sockaddr* pstLocalAddr, socklen_t addrlen) noexcept
{
    //set address reuse
    int iReuseAddr = 1;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &iReuseAddr, sizeof(int));
    return ::bind(_sockfd, pstLocalAddr, sizeof(sockaddr));
}

int Socket::send(const void* pBuf, size_t iLen, int iFlags)
{
    return (int)::send(_sockfd, pBuf, iLen, iFlags);
}

int Socket::recv(void* pBuf, size_t iLen, int iFlags)
{
    return (int)::recv(_sockfd, pBuf, iLen, iFlags);
}

int Socket::sendto(const void* pBuf, size_t iLen,
                   const std::string sToAddr, uint16_t uToPort, int iFlags)
{
    sockaddr stToAddr;
    this->getSockAddr(sToAddr, uToPort, stToAddr);
    
    if (sToAddr == "")
    {
        sockaddr_in* p = (sockaddr_in*)&stToAddr;
        p->sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }
    
    return sendto(pBuf, iLen, stToAddr, iFlags);
}

int Socket::sendto(const void* pBuf, size_t iLen, const char* sUnixPath, int iFlags)
{
    sockaddr stToAddr;
    this->getUnixAddr(sUnixPath, stToAddr);
    return sendto(pBuf, iLen, stToAddr, iFlags);
}

int Socket::sendto(const void* pBuf, size_t iLen, sockaddr& stToAddr, int iFlags)
{
    return (int)::sendto(_sockfd, pBuf, iLen, iFlags, &stToAddr, sizeof(sockaddr));
}

int Socket::recvfrom(void* pBuf, size_t iLen, sockaddr& stFromAddr, int iFlags)
{
    socklen_t iAddrLen = sizeof(sockaddr);
    return (int)::recvfrom(_sockfd, pBuf, iLen, iFlags, &stFromAddr, &iAddrLen);
}

void Socket::setBlock(bool bBlock)
{
    int flag = 0;
    if ((flag = fcntl(_sockfd, F_GETFL, 0)) == -1)
    {
        //throw
    }
    if (bBlock)
    {
        flag &= ~O_NONBLOCK;
    }
    else
    {
        flag |= O_NONBLOCK;
    }
    if(fcntl(_sockfd, F_SETFL, flag) == -1)
    {
        //throw
    }
}

int TcpClient::send(const char* sBuf, size_t iLen)
{
    int iRet = _checkConnect();
    if (iRet < 0)
    {
        return iRet;
    }
    
    iRet = _sock.send(sBuf, (int)iLen);
    if (iRet < 0)
    {
        _sock.close();
        return EM_SEND;
    }
    
    return EM_SUCCESS;
}

int TcpClient::recv(const char* sBuf, size_t& iLen)
{
    int iRet = _checkConnect();
    if (iRet < 0)
    {
        return iRet;
    }
    //select
    int iRecv = _sock.recv((void*)sBuf, iLen);
    if (iRecv < 0)
    {
        _sock.close();
        return EM_RECV;
    }
    else if (iRecv == 0)
    {
        _sock.close();
        return EM_CLOSE;
    }
    iLen = (size_t)iRecv;
    return EM_SUCCESS;
}

int TcpClient::_checkConnect()
{
    if (_sock.isValid())
        return EM_SUCCESS;
    __TRY__
    if (_port == 0)
    {
        _sock.create(SOCK_STREAM, AF_UNIX);
    }
    else
    {
        _sock.create(SOCK_STREAM, AF_INET);
    }
    //_sock.setblock(false)
    if (_port == 0)
    {
        _sock.connect(_ip.c_str());
    }
    else
    {
        _sock.connect(_ip, _port);
    }
    return EM_SUCCESS;//success if no throw
    __CATCH__(Socket::ConnectException& e)
    _sock.close();
    return EM_CONNECT;
    __CATCH__(Socket::Exception &e)
    _sock.close();
    return EM_SOCKINIT;
    __END__
}