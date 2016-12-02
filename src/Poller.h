//
//  Poller.h
//
//  by jiahuan.liu
//  11/28/2016
//
#ifndef _EPOLLER_H
#define _EPOLLER_H

#include "Common.h"
#include "Exception.h"

#include <poll.h>
#include <sys/types.h>
#include <sys/time.h>

const int MAX_CONN = 2000;
const int READ_EVENT = POLLIN;
const int WRITE_EVENT = POLLOUT;

class PollerBase : public std::enable_shared_from_this<PollerBase>, public Noncopyable
{
public:
    struct Exception: public ::Exception
    {
        Exception(const std::string& strerr) : ::Exception(strerr) {}
        Exception(const std::string& strerr, int ierr)
        : ::Exception(strerr, ierr) {}
        ~Exception() noexcept {}
    };
public:
    PollerBase() {}
    ~PollerBase() {}
    virtual void create(int iMaxConn = MAX_CONN) = 0;
    virtual void add(int fd, void* data, uint32_t events) = 0;
    virtual void del(int fd, void* data, uint32_t events) = 0;
    virtual void mod(int fd, void* data, uint32_t events) = 0;
    virtual int wait(int iMs) = 0;
private:
    virtual void _ctrl(int fd, void* data, uint32_t events, uint32_t op) = 0;
protected:
    int _fd;
    int _maxConn;
};

#endif