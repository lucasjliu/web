//
//  ThreadSync.cpp
//
//  by jiahuan.liu
//  11/20/2016
//

#include "ThreadSync.h"

void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock{_mutex};
    if (--_count<0)
    {
        _cond.wait(lock, [&]()->bool{return _nnotify > 0;});
        --_nnotify;
    }
}

void Semaphore::notify()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(++_count<=0)
    {
        ++_nnotify;
        _cond.notify_one();
    }
}

void Monitor::lock() const
{
    _mutex.lock();
    _nnotify = 0;
}

void Monitor::unlock() const
{
    notifyImpl(_nnotify);
    _mutex.unlock();
}

bool Monitor::trylock() const
{
    bool ret = _mutex.try_lock();
    if (ret)
    {
        _nnotify = 0;
    }
    return ret;
}

void Monitor::wait() 
{
    notifyImpl(_nnotify);
    
    std::unique_lock<Mutex> lock{_mutex};
    _cond.wait(lock);
    
    _nnotify = 0;
}

bool Monitor::timewait(int ms)
{
    notifyImpl(_nnotify);
    
    std::unique_lock<Mutex> lock{_mutex};
    bool noTimeout = _cond.wait_for(lock, std::chrono::milliseconds(ms))
        == std::cv_status::no_timeout;
    
    _nnotify = 0;
    
    return noTimeout;
}

void Monitor::notify() const
{
    if (_nnotify != -1)
    {
        _nnotify++;
    }
}

void Monitor::notifyAll() const
{
    _nnotify = -1;
}

void Monitor::notifyImpl(int nnotify) const
{
    if (nnotify)
    {
        if (nnotify == -1)
        {
            _cond.notify_all();
        }
        else
        {
            while (--nnotify)
            {
                _cond.notify_one();
            }
        }
    }
}

RWLock::RWLock()
{
    int ret = ::pthread_rwlock_init(&_rwlock, NULL);
    
    if(ret != 0)
    {
        throw Exception("[RWLock::RWLock] pthread_rwlock_init error", ret);
    }
}

RWLock::~RWLock()
{
    int ret = ::pthread_rwlock_destroy(&_rwlock);
    if(ret != 0)
    {
        throw Exception("[RWLock::~RWLock] pthread_rwlock_destroy error:"
                        + std::string(strerror(ret)));
    }
}

void RWLock::rlock() const
{
    int ret = ::pthread_rwlock_rdlock(&_rwlock);
    if(ret != 0)
    {
        if(ret == EDEADLK)
        {
            throw Exception("[RWLock::rlock] pthread_rwlock_rdlock dead lock error", ret);
        }
        else
        {
            throw Exception("[RWLock::rlock] pthread_rwlock_rdlock error", ret);
        }
    }
}


void RWLock::wlock() const
{
    int ret = ::pthread_rwlock_wrlock(&_rwlock);
    if(ret != 0)
    {
        if(ret == EDEADLK)
        {
            throw Exception("[RWLock::wlock] pthread_rwlock_wrlock dead lock error", ret);
        }
        else
        {
            throw Exception("[RWLock::wlock] pthread_rwlock_wrlock error", ret);
        }
    }
}

void RWLock::tryRlock() const
{
    int ret = ::pthread_rwlock_tryrdlock(&_rwlock);
    if(ret != 0)
    {
        if(ret == EDEADLK)
        {
            throw Exception("[RWLock::tryRlock] pthread_rwlock_tryrdlock dead lock error", ret);
        }
        else
        {
            throw Exception("[RWLock::tryRlock] pthread_rwlock_tryrdlock error", ret);
        }
    }
}

void RWLock::tryWlock() const
{
    int ret = ::pthread_rwlock_trywrlock(&_rwlock);
    if(ret != 0)
    {
        if(ret == EDEADLK)
        {
            throw Exception("[RWLock::tryWlock] pthread_rwlock_trywrlock dead lock error", ret);
        }
        else
        {
            throw Exception("[RWLock::tryWlock] pthread_rwlock_trywrlock error", ret);
        }
    }
    
}

void RWLock::unlock() const
{
    int ret = ::pthread_rwlock_unlock(&_rwlock);
    if(ret != 0)
    {
        throw Exception("[RWLock::unlock] pthread_rwlock_unlock error", ret);
    }
}