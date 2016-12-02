//
//  ThreadSync.h
//
//  by jiahuan.liu
//  11/20/2016
//
#ifndef _THREADSYNC_H
#define _THREADSYNC_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

#include <pthread.h>

#include "Common.h"

class Semaphore : public Noncopyable
{
public:
    Semaphore(int value=0): _count{value}, _nnotify{0} {}
    void wait();
    void notify();
private:
    int _count;
    int _nnotify;
    std::mutex _mutex;
    std::condition_variable _cond;
};

class Monitor : public Noncopyable
{
    typedef std::mutex Mutex;
    typedef std::condition_variable Cond;
public:
    Monitor() :_nnotify(0) {}
    virtual ~Monitor() {}
    
    void lock() const;
    void unlock() const;
    bool trylock() const;
    void wait() ;
    bool timewait(int ms);
    void notify() const;
    void notifyAll() const;
protected:
    void notifyImpl(int nnotify) const;
protected:
    mutable Mutex   _mutex;
    mutable Cond    _cond;
    mutable int     _nnotify;
};

typedef Monitor ThreadLock;

#include "Exception.h"

class RWLock : public Noncopyable
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
    RWLock();
    ~RWLock();
    void rlock() const;
    void wlock() const;
    void tryRlock() const;
    void tryWlock() const ;
    void unlock() const;
private:
    mutable pthread_rwlock_t _rwlock;
};

template<class T>
class LockT : public Noncopyable
{//TODO: acquire
public:
    LockT(const T& mutex) : _mutex(mutex)
    {
        _mutex.lock();
    }
    virtual ~LockT()
    {
        _mutex.unlock();
    }
protected:
    const T& _mutex;
};

typedef LockT<ThreadLock> Lock;

template<class T>
class RLockT : public Noncopyable
{
public:
    RLockT(const T& mutex) :_mutex(mutex)
    {
        _mutex.rlock();
    }
    virtual ~RLockT()
    {
        _mutex.unlock();
    }
protected:
    const T& _mutex;
};

typedef RLockT<RWLock> RLock;

template<class T>
class WLockT : public Noncopyable
{
public:
    WLockT(const T& mutex) :_mutex(mutex)
    {
        _mutex.wlock();
    }
    virtual ~WLockT()
    {
        _mutex.unlock();
    }
protected:
    const T& _mutex;
};

typedef WLockT<RWLock> WLock;

#endif//_THREADSYNC_H