//
//  SearchServer.h
//
//  by jiahuan.liu
//  12/15/2016
//
#ifndef _SINGLETON_H
#define _SINGLETON_H

#include <cstdlib>

#include "ThreadSync.h"

template <typename T>
class Singleton
{
public:
    T* getInstance()
    {
        if (!_pInstance)
        {
            Lock(_lock);
            if (!_pInstance)
            {
                _pInstance = new T();
                std::atexit([=] {
                    if (_pInstance) {
                        delete _pInstance;
                }});
            }
        }
    }
private:
    static T*           _pInstance;
    static ThreadLock   _lock;
};

template <typename T>
T* Singleton<T>::_pInstance = nullptr;

template <typename T>
ThreadLock Singleton<T>::_lock;

#endif //_SINGLETON_H