//
//  Socket.h
//
//  by jiahuan.liu
//  10/12/2016
//

#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "Logger.h"
#include "Exception.h"

class Socket
{
public:
    struct Exception : ::Exception
    {
        Exception(const std::string& strerr) : ::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : ::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
    struct ConnectException : Exception
    {
        ConnectException(const std::string& strerr) : Exception(strerr) {}
        ConnectException(const std::string& strerr, int ierr)
        : Exception(strerr, ierr) {}
        ~ConnectException() noexcept {}
    };
public:
    Socket();
    ~Socket();
    
    //life cycle
    void init(int fd, bool isOwner, int iFamily);
    void close();
    void shutdown(int iHow);
    void create(int iSocketType, int iFamily);
    
    //private member interface
    inline void setOwner(bool isOwner);
    inline void setFamily(int iFamily);
    inline int getfd();
    inline bool isValid();
    
    //basic socket interface
    //void connect(const sockaddr& stServerAddr);
    void connect(const std::string& sServerAddr, uint16_t uPort);
    void connect(const char* sUnixPath);
    
    //no neeed
    void bind(sockaddr* stLocalAddr);
    void bind(const uint16_t& uLocalPort, const std::string& sLocalAddr = "");
    void bind(const char* sUnixPath);
    void listen(int iBackLog);
    
    //more simple version?
    int accept(Socket& remoteSockfd, sockaddr* stRemoteAddr, socklen_t& uSocklen);
    //int accept(Socket& remoteSockfd, sockaddr stLocalAddr);
    //int accept(Socket& remoteSockfd, string& sAddr, uint16_t& uPort);
    
    //should be private
    void getSockAddr(const std::string& sSockAddr, const uint16_t& uPort,
                      sockaddr& stSockAddr) const;
    void getUnixAddr(const char* sUnixPath, sockaddr& stUnixAddr) const;
    
    //socket I/O
    int send(const void* pBuf, size_t iLen, int iFlags = 0);
    int recv(void* pBuf, size_t iLen, int iFlags = 0);
    int sendto(const void* pBuf, size_t iLen,
               const std::string sToAddr, uint16_t uToAddr, int iFlags = 0);
    int sendto(const void* pBuf, size_t iLen, const char* sUnixPath, int iFlags = 0);
    int sendto(const void* pBuf, size_t iLen, sockaddr& stToAddr, int iFlags = 0);
    int recvfrom(void* pBUf, size_t iLen, sockaddr& stFromAddr, int iFlags = 0);
    
    //socket setting
    void setBlock(bool bBlock);
    void setSockOpt();
    
private:
    void _getHostByName(const std::string& sAddr, in_addr& stInAddr) const;
    void _parseAddr(const std::string& sAddr, in_addr& stInAddr) const;
    int _connect(const std::string& sServerAddr, uint16_t uPort) noexcept;
    int _connect(const char* sLocalPath) noexcept;
    inline int _connect(sockaddr* pstServerAddr, socklen_t addrlen) noexcept;
    int _bind(uint16_t uLocalPort, const std::string& sBindAddr) noexcept;
    int _bind(const char* sLocalPath) noexcept;
    inline int _bind(sockaddr* pstBindAddr, socklen_t addrlen) noexcept;
protected:
    static const int INVALID_SOCKET = -1;
    int _sockfd;
    int _isOwner;
    int _iFamily;
};

class ClientSocket
{
    //ADT
public:
    enum
    {
        EM_SUCCESS  = 0,
        EM_SEND     = -1,
        EM_SELECT   = -2,
        EM_TIMEOUT  = -3,
        EM_RECV     = -4,
        EM_CLOSE    = -5,
        EM_CONNECT  = -6,
        EM_SOCKINIT = -7
    };
    ClientSocket() :_port(0), _timeout(3000) {}
    ClientSocket(const std::string& sIp, int iPort, int iTimeout) {init(sIp, iPort, iTimeout);}
    virtual ~ClientSocket() {}
    void init(const std::string& sIp, int iPort, int iTimeout)
    {
        _sock.close();
        _ip      = sIp;
        _port    = iPort;
        _timeout = iTimeout;
    }
    virtual int send(const char* sBuf, size_t iLen) = 0;
    virtual int recv(const char* sBuf, size_t& iLen) = 0;
protected:
    Socket      _sock;
    std::string _ip;
    uint16_t    _port;
    int         _timeout;
};

class TcpClient : public ClientSocket
{
public:
    TcpClient() {}
    virtual ~TcpClient() {}
    TcpClient(const std::string& sIp, int iPort, int iTimeout) :ClientSocket(sIp, iPort, iTimeout) {}
    int send(const char* sBuf, size_t iLen);
    int recv(const char* sBuf, size_t& iLen);
protected:
    int _checkConnect();
};

#endif//_SOCKET_H