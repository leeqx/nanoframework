#include <string.h>
#include <errno.h>

#include "common/global.h"
#include "net.h"

CNet::CNet(int port,int listenSize):m_listenFd(-1),m_listendSize(5),m_epollFd(-1)
{}
CNet::~CNet(){}
/**
 * set fd as non-block module
 */
int CNet::SetNonBlock(int fd)
{
    if(fd < 0)
    {
        LOG(ERROR,"invalied fd");
        return -1;
    }

    int flag = fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,flag | O_NONBLOCK);
    return 0;
}
/**
 * set linger:SOL_LINGER
 * onOff:0-off ,none 0- on
 */
int CNet::SetLinger(int fd,int onOff,int time)
{
    linger ling = {onOff,time};

    if(0 != setsockopt(fd,SOL_SOCKET,SO_LINGER,(char*)&ling,sizeof(ling)))
    {
        LOG(ERROR,"set lingger failed:%s",strerror(errno));
        return -1;
    }
    return 0;
}
int CNet::SetKeepalive(int fd,bool alive,int idleTime,int interval,int count)
{
    if(fd < 0)
    {
        LOG(ERROR,"invalide fd");
        return -1;
    }
    int keepAlive = alive;
    int keepIdle  = idleTime;
    int keepIntv  = interval;  
    int keepCount = count;
    
    setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepAlive,sizeof(keepAlive));
    setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,(char*)&keepIdle,sizeof(keepIdle));
    setsockopt(fd,SOL_TCP,TCP_KEEPINTERVAL,(char*)&keepIntv,sizeof(keepIntv));
    setsockopt(fd,SOL_TCP,TCP_KEEPCNT,(char*)&keepCount,sizeof(keepCount));

    return 0;
}
/**
 *SOL_REUSEADDR
 */
int CNet::SetReuseAddr(int fd,bool onOff)
{
    if(fd < 0)
    {
        LOG(ERROR,"invalied fd");
        return -1;
    }
    bool isReuseAddr = onOff;
    if(0 != setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*)&isReuseAddr,sizeof(isReuseAddr)))
    {
        LOG(ERROR,"set reuseaddr failed:%s",strerror(errno)) ;
        return -1;
    }

    return 0;
}
/**
 * bufSize:byte
 */
int CNet::SetSendBufferSize(int fd,int bufSize)
{

}
int CNet::SetRecvBufferSize(int fd,int bufSize)
{
}
/**
 *EPOLLIN|EPOLLOUT|EPOLLREHUP|EPOLLPRI|EPOLLET|EPOLLERR|EPOLLHUP|EPOLLONESHOT
 */
int CNet::UpdateEpollEvent(int level)
{

}

int CNet::CheckTCPConfig()
{

}

int CNet::initSocket(int size)
{
    m_listenFd = socket(AF_INET,SOCK_STREAM,0);
    if ( m_listenFd < 0)
    {
        LOG(ERROR,"Create socket failed:%s",strerror(errno));
        return -1;
    }
    struct sockaddr_in serverAddr;
    bzero(&serverAddr,sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if(0 != bind(m_listenFd,(sockaddr*)&serverAddr,sizeof(serverAddr)))
    {
        LOG(ERROR,"bind to address failed:%s",strerror(errno));
        return -1;
    }
    if(0 != listen(m_listenFd,m_listendSize))
    {
        LOG(ERROR,"listen failed:%s",strerror(errno));
        return -1;
    }
    return 0;
}
/**
 * create epoll and add  m_listendFd
 */
int CNet::CreateEpoll(int size/*=65535*/)
{
    if(0 != m_epollFd)
    {
        close(m_epollFd);
    }
    if(0 != (m_epollFd = epoll_create(size)) )
    {
        LOG(ERROR,"create epoll failed:%s\n",strerror(errrno));
        return -1;
    }

    AddEpoll(m_listenFd,EPOLLIN|EPOLLET);

    return 0;
}
int CNet::AddEpoll(int fd,int fflag)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = fflag;
    if(0 != epoll_ctl(m_epollFd,EPOLL_CTL_ADD,fd,&event))
    {
        LOG(ERROR,"add epoll event failed:%s",strerror(errno));
        return -1;
    }
    return 0;
}
int CNet::DelEpoll(int fd,int fflag)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.data = fflag;
    if(0 != epoll_ctl(m_epollFd,EPOLL_CTL_DEL,fd,&event))
    {
        LOG(ERROR,"delete epoll event failed:%s",strerror(errno));
        return -1;
    }
    return 0;

}
int CNet::ModEpoll(int fd,int fflag)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.event = fflag;

    if(0 != epoll_ctl(m_epollFd,EPOLL_CTL_MOD,fd,&event))
    {
        LOG(ERROR,"delete epoll event failed:%s",strerror(errno));
        return -1;
    }
    return 0;
}
int CNet::EpollWait()
{

    int max = 0;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientaddr);

    for(;;)
    {
        max = epoll_wait(m_epollFd,m_events,MAX_EVENTS_SIZE,-1);
        if(max > 0)
        {
            for(int i = 0;i < max;i++)
            {
                if(m_events[i].data.fd == m_listenFd)
                {// accept client connect
#ifndef _GNU_SOURCE
                    int clientFd = accept(m_listendFd,(struct sockaddr*)&clientaddr,&clientAddrLen);
                    SetNonBlock(clientFd);
#else
                    // clientFd will set to :non-block
                    int clientFd = accept4(m_listenFd,(struct sockaddr*)&clientaddr,&clientAddrLen,SOCK_NONBLOCK);
#endif
                    if(clientFd < 0)
                    {
                        LOG(ERROR,"accept client request failed:%s",strerror(errno));
                        continue;
                    }
                    // add to epoll events
                    AddEpoll(clientFd,EPOLLIN|EPOLLET);
                }
                else if(m_events[i].event & EPOLLIN) // read event
                {
                 
                }
                else if(m_events[i].event & EPOLLOUT) // write event
                {
                
                }
            }
        
        }
        else if(-1 == max)
        {
            LOG(ERROR,"epoll_wait return failed:%s",strerror(errno));
        }
    
    }

}
/**
 * 将数据分发到后端，提供给后端线程处理
 */
int CNet::Dispatch2BackQueue()
{

}


