#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <fcntl.h>

class CNet
{
    public:
        CNet(int,int);
        virtual ~CNet();
    public:
        /**
         * set fd as non-block module
         */
        int SetNonBlock(int fd);
        /**
         * set linger:SOL_LINGER
         */
        int SetLinger(int fd,int onOff,int time);
        /**
         * alive:true-open keepalive
         * idleTime(s):if idle time no packet will send keepalive package
         * interval(s): interval to send new keepavlie package if last one is failed(cann't recive ack
         * count:total send count keepalive packages
         */
        int SetKeepalive(int fd,bool alive=true,int idleTime=30,int interval=3,int count=3);
        /**
         *SOL_REUSEADDR
         */
        int SetReuseAddr(int fd,bool onOff);
        /**
         * SO_SNDBUF
         */
        int SetSendBufferSize(int fd,int bufSize);
        /**
         * SO_RCVBUF
         */
        int SetRecvBufferSize(int fd,int bufSize);

        /**
         *EPOLLIN|EPOLLOUT|EPOLLREHUP|EPOLLPRI|EPOLLET|EPOLLERR|EPOLLHUP|EPOLLONESHOT
         */
        int UpdateEpollEvent(int level);

        int CheckTCPConfig();

        int initSocket(int port,int size);
        int CreateEpoll(int size=65535);
        int AddEpoll(int fd,int fflag);
        int DelEpoll(int fd,int fflag);
        int ModEpoll(int fd,int fflag);
        int EpollWait();

        int OnRead(int fd);
        int OnWrite(int fd);
        /**
         *讲处理函数与fd绑定
         */
        int AttachHandler(int fd,void*(*func)(void*));

        /**
         * 讲数据分发到后端，提供给后端线程处理
         */
        int Dispatch2BackQueue();
    private:
        int m_listenFd;
        int m_epollFd;
        int m_port;
        int m_listendSize;
        struct epoll_event m_events[65535];
};


