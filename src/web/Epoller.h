//
//  Epoller.h
//
//  by jiahuan.liu
//  10/22/2016
//
#ifndef _EPOLLER_H
#define _EPOLLER_H

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

class Epoller
{
public:
    Epoller();
    ~Epoller();
    void create(int iMaxConn);
    void add(int iSock);
    void del(int iSock);
    void wait(int iTimeout);
    void ctl();
};

#endif
