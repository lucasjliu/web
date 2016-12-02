//
//  Poller.cpp
//
//  by jiahuan.liu
//  11/28/2016
//
#include <unistd.h>

#include "Poller.h"
#include "Logger.h"

#ifdef OS_LINUX
#include <sys/epoll.h>
#elif defined(__APPLE__) || defined(MACH)
#include <sys/event.h>
#else
#error "platform unsupported"
#endif

#ifdef OS_LINUX
class Epoller : public PollerBase
{
public:
    Epoller();
    ~Epoller();
    void create(int iMaxConn = MAX_CONN) override;
    void add(int fd, void* data, uint32_t events) override;
    void del(int fd, void* data, uint32_t events) override;
    void mod(int fd, void* data, uint32_t events) override;
    int wait(int iMs) override;
private:
    void _ctrl(int fd, void* data, uint32_t events, uint32_t op) override;
private:
    bool _et;
    epoll_event* _actEv;
};

PollerBase* newInstance() {return new Epoller();}

#elif defined(__APPLE__) || defined(MACH)
class Kqueue : public PollerBase
{
public:
    Kqueue();
    ~Kqueue();
    void create(int iMaxConn = MAX_CONN) override;
    void add(int fd, void* data, uint32_t events) override;
    void del(int fd, void* data, uint32_t events) override;
    void mod(int fd, void* data, uint32_t events) override;
    int wait(int iMs) override;
    struct kevent& get(int i) const {return _actEv[i];}
private:
    void _ctrl(int fd, void* data, uint32_t events, uint32_t op) override;
private:
    struct kevent* _actEv;
};

PollerBase* newInstance() {return new Kqueue();}

Kqueue::Kqueue()
{
    _fd = -1;
    _actEv = nullptr;
    _maxConn = MAX_CONN;
}

Kqueue::~Kqueue()
{
    if (_actEv)
    {
        delete[] _actEv;
        _actEv = nullptr;
    }
    if (_fd > 0)
    {
        ::close(_fd);
    }
}

void Kqueue::create(int iMaxConn)
{
    _fd = kqueue();
    if (_fd < 0)
    {
        LOG_FATAL << "kqueue init error";
        throw Exception("kqueue init error");
    }
    
    _maxConn = iMaxConn;
    
    if (_actEv)
    {
        delete[] _actEv;
    }
    
    _actEv = new struct kevent[_maxConn];
}

void Kqueue::_ctrl(int fd, void* data, uint32_t events, uint32_t op)
{
    struct timespec now;
    now.tv_nsec = 0;
    now.tv_sec = 0;
    struct kevent ev[2];
    int n = 0;
    if (events & READ_EVENT)
    {
        EV_SET(&ev[n++], fd, EVFILT_READ, op, 0, 0, data);
    }
    if (events & WRITE_EVENT)
    {
        EV_SET(&ev[n++], fd, EVFILT_WRITE, op, 0, 0, data);
    }
    if (kevent(_fd, ev, n, NULL, 0, &now))
    {
        LOG_FATAL << "kqueue ctrl error";
        throw Exception("kqueue ctrl error");
    }
}

void Kqueue::add(int fd, void* data, uint32_t events)
{
    _ctrl(fd, data, events, EV_ADD|EV_ENABLE);
}

void Kqueue::del(int fd, void* data, uint32_t events)
{
    _ctrl(fd, data, events, EV_DELETE);
}

void Kqueue::mod(int fd, void* data, uint32_t events)
{
    _ctrl(fd, data, events, EV_ADD|EV_ENABLE);
}

int Kqueue::wait(int iMs)
{
    struct timespec timeout;
    timeout.tv_sec = iMs / 1000;
    timeout.tv_nsec = (iMs % 1000) * 1000 * 1000;
    return kevent(_fd, NULL, 0, _actEv, _maxConn, &timeout);
}

#endif