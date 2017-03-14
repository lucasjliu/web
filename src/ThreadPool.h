//
//  ThreadPool.h
//
//  by jiahuan.liu
//  10/31/2016
//

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <atomic>
#include <assert.h>
#include <utility>
#include <cstdlib>
#include <functional>
#include <thread>

#include "Common.h"

template<typename T>
class LFQueue : public Noncopyable
{
    struct Node
    {
        T                   data;
        std::atomic<size_t> seq;//position on the ring buffer
    };
    typedef Node* NodePtr;
    typedef std::atomic<size_t> Seq;

public:
    LFQueue(size_t size);

    ~LFQueue();

    bool push(const T& data);

    bool pop(T& data);

    template<typename... Args>  
    bool emplace(Args&&... args);

    size_t size() const;

protected:
    //ring buffer
    std::function<size_t(const Seq&)> _trunc;

    static constexpr size_t iCacheLine = 64;

    #define Alignas alignas(iCacheLine)

private:
    const size_t    _iSize;
    const size_t    _iMask;
    NodePtr         _pBuf;

    //frequently modified data
    Alignas Seq     _seqHead;
    Alignas Seq     _seqTail;

    //separate _seqTail from adjacent allocations
    char            _padding[iCacheLine - sizeof(_seqTail)];
};

class ThreadPool
{
    typedef std::function<void()> Task;
public:
    ThreadPool(size_t thNum, size_t qSize, bool bStart = true);
    ~ThreadPool() {}

    void start();
    void stop();
    bool isStop();
    void join();

    template<typename Func, typename... Args>
    bool addTask(Func&& func, Args&&... args);

    template<typename Func, typename... Args>
    bool addTask(const Func& func, const Args&... args);

private:
    std::vector<std::thread> _threads;
    LFQueue<Task>            _tasks;
    std::atomic<bool>        _bStop;
};

template<typename Func, typename... Args>
bool ThreadPool::addTask(Func&& func, Args&&... args)
{
    if (isStop())
    {
        return false;
    }
    //using return_type = typename std::result_of<Func(Args...)>::type;
    return _tasks.push(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
}

template<typename Func, typename... Args>
bool ThreadPool::addTask(const Func& func, const Args&... args)
{
    if (isStop())
    {
        return false;
    }
    //using return_type = typename std::result_of<Func(Args...)>::type;
    return _tasks.push(std::bind(func, args...));
}

template<typename T>
LFQueue<T>::LFQueue(size_t size)
:_iSize(size),
_iMask(size-1),
_pBuf(new Node[size]),
_seqHead(0),
_seqTail(0)
{
    if ((_iSize != 0) && ((_iSize & (_iSize + 1)) == _iSize))
    {
        _trunc = [&](const Seq& seq){return seq & _iMask;};
    }
    else
    {
        _trunc = [&](const Seq& seq){return seq % _iSize;};
    }
    for (size_t i = 0; i < _iSize; i++)
    {
        _pBuf[i].seq.store(i, std::memory_order_relaxed);
    }
}

template<typename T>
LFQueue<T>::~LFQueue()
{
    delete []_pBuf;
}

template<typename T>
bool LFQueue<T>::push(const T& data)
{
    size_t head;
    while(true)
    {
        head = _seqHead.load(std::memory_order_relaxed);
        
        NodePtr node = _pBuf + _trunc(head);
        //NodePtr node = &_pBuf[_trunc(head)];
        size_t curr = node->seq.load(std::memory_order_acquire);
        
        intptr_t diff = (intptr_t)curr - (intptr_t)head;
        
        if (diff == 0)
        {//node points to head node
            if (_seqHead.compare_exchange_weak(head, head+1, std::memory_order_relaxed))
            {//CAS
                node->data = data;
                node->seq.store(head+1, std::memory_order_release);
                return true;
            }
        }
        else if (diff < 0)
        {//buffer is full since last dequeue has incremented head node
            return false;
        }
        //otherwise repeat since head is modified in critical section
    }
}

template<typename T>
template<typename... Args>  
bool LFQueue<T>::emplace(Args&&... args)
{
    return push(T(std::forward<Args>(args)...));
}

template<typename T>
bool LFQueue<T>::pop(T& data)
{
    size_t tail;
    while(true)
    {
        tail = _seqTail.load(std::memory_order_relaxed);
        
        NodePtr node = _pBuf + _trunc(tail);
        size_t curr = node->seq.load(std::memory_order_acquire);
        
        intptr_t diff = (intptr_t)curr - (intptr_t)(tail + 1);
        
        if (diff == 0)
        {
            if (_seqTail.compare_exchange_weak(tail, tail + 1, std::memory_order_release))
            {
                data = node->data;
                node->seq.store(tail + _iSize, std::memory_order_release);
                return true;
            }
        }
        else if (diff < 0)
        {//buffer is empty since curr == tail
            return false;
        }
    }
}

template<typename T>
size_t LFQueue<T>::size() const
{
    size_t head, tail;
    while (true)
    {
        head = _seqHead.load(std::memory_order_relaxed);
        tail = _seqTail.load(std::memory_order_relaxed);
        if (head == _seqHead.load(std::memory_order_relaxed))
            return head - tail;
    }
}

#endif//_THREADPOOL_H
