#include <string.h>
#include <errno.h>

#include "common/global.h"
#include "net.h"
#include "msghandler/handler_factory.h"

CNet::CNet(int listenSize):m_listenFd(-1),m_listendSize(listenSize),m_epollFd(-1)
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
    setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,(char*)&keepIntv,sizeof(keepIntv));
    setsockopt(fd,SOL_TCP,TCP_KEEPCNT,(char*)&keepCount,sizeof(keepCount));

    return 0;
}
/**
 *SOL_REUSEADDR
 */
int CNet::SetReuseAddr(int fd,int onOff)
{
    if(fd < 0)
    {
        LOG(ERROR,"invalied fd");
        return -1;
    }
    int isReuseAddr = onOff;
    if(0 != setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const char*)&isReuseAddr,sizeof(isReuseAddr)))
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

int CNet::initSocket(int port,int epollSize)
{
    m_port = port;
    m_listenFd = socket(AF_INET,SOCK_STREAM,0);
    if ( m_listenFd <=0)
    {
        LOG(ERROR,"Create socket failed:%s",strerror(errno));
        return -1;
    }
    this->SetReuseAddr(m_listenFd,true);
    this->SetNonBlock(m_listenFd);
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
    this->CreateEpoll(epollSize);
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
    if(-1 == (m_epollFd = epoll_create(size)) )
    {
        LOG(ERROR,"create epoll failed:%s\n",strerror(errno));
        return -1;
    }

    AddEpoll(m_listenFd,EPOLLIN|EPOLLET,NULL);

    return 0;
}
int CNet::AddEpoll(int fd,long unsigned int fflag,int (CNet::*pFdFunc)(int))
{
    struct EpollData *pData = CreateEpollData(fd,pFdFunc);

    struct epoll_event event;
    event.events   = fflag;
    event.data.ptr = (void*)pData;
    if(0 != epoll_ctl(m_epollFd,EPOLL_CTL_ADD,fd,&event))
    {
        LOG(ERROR,"add epoll event failed:%s",strerror(errno));
        return -1;
    }
    LOG(INFO,"add %d to epoll,events:%lu",fd,fflag);
    return 0;
}
int CNet::DelEpoll(struct epoll_event &event,long unsigned int fflag)
{
    event.events  = fflag;
    if(0 != epoll_ctl(m_epollFd,EPOLL_CTL_DEL,GetFdFromEpollData((struct EpollData*)(event.data.ptr)),&event))
    {
        LOG(ERROR,"delete epoll event failed:%s",strerror(errno));
        return -1;
    }
    if(event.data.ptr != NULL)
    {
        close(GetFdFromEpollData((struct EpollData*)(event.data.ptr)));
        free(event.data.ptr);
        event.data.ptr = NULL;
    }
    return 0;
}
int CNet::ModEpoll(int fd,long unsigned int fflag,int(CNet::*pFdFunc)(int))
{
    struct EpollData *pData = CreateEpollData(fd,pFdFunc);

    struct epoll_event event;
    event.data.ptr = (void*)pData;
    event.events   = fflag;

    LOG(INFO,"Mod %d to epoll,events:%lu",fd,fflag);
    if(0 != epoll_ctl(m_epollFd,EPOLL_CTL_MOD,fd,&event))
    {
        LOG(ERROR,"Mod %d epoll event failed:%s",fd,strerror(errno));
        return -1;
    }
    return 0;
}
int CNet::EpollWait()
{
    int max = 0;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = 0;
    LOG(INFO,"begin handle net msg,port:%d",m_port);
    for(;;)
    {
        max = epoll_wait(m_epollFd,m_events,MAX_EVENTS_SIZE,-1);
        if(max > 0)
        {
            int curFd = -1;
            for(int i = 0;i < max;i++)
            {
                curFd = GetFdFromEpollData((struct EpollData*)(m_events[i].data.ptr));
                if(curFd == m_listenFd)
                {// accept client connect
#ifndef _GNU_SOURCE
                    clientFd = accept(m_listendFd,(struct sockaddr*)&clientAddr,&clientAddrLen);
                    SetNonBlock(clientFd);
#else
                    // clientFd will set to :non-block
                    clientFd = accept4(m_listenFd,(struct sockaddr*)&clientAddr,&clientAddrLen,SOCK_NONBLOCK);
#endif
                    char szIp[33]={0};
                    LOG(DEBUG,"new client connect,fd[%d]:%s\n",clientFd,inet_ntop(AF_INET,(void*)&(clientAddr.sin_addr),szIp,clientAddrLen));
                    if(clientFd <= 0)
                    {
                        LOG(ERROR,"accept client request failed:%s",strerror(errno));
                        continue;
                    }
                    // add to epoll events
                    AddEpoll(clientFd,EPOLLIN|EPOLLET,&CNet::OnRead);
                }
                else if(m_events[i].events & EPOLLIN) // read event
                {
                    struct EpollData *pCurEpollData = (struct EpollData*)(m_events[i].data.ptr);
                    if((this->*pCurEpollData->rFunc)(curFd)<=0)
                    {// client close socket
                        DelEpoll(m_events[i],EPOLLIN|EPOLLET);
                    }
                    else
                    {
                        ModEpoll(curFd,EPOLLOUT|EPOLLET,&CNet::OnWrite);
                    }
                }
                else if(m_events[i].events & EPOLLOUT) // write event
                {
                    struct EpollData *pCurEpollData = (struct EpollData*)(m_events[i].data.ptr);
                    if((this->*pCurEpollData->wFunc)(curFd)<=0)
                    {
                        DelEpoll(m_events[i],EPOLLOUT|EPOLLET);
                    }
                    else
                    {
                        ModEpoll(curFd,EPOLLIN|EPOLLET,&CNet::OnRead);
                    }
                }
            }
        
        }
        else if(-1 == max)
        {
            LOG(ERROR,"epoll_wait return failed:%s",strerror(errno));
            break;
        }
    }

}

int CNet::OnRead(int fd)
{
    if(fd < 0)
    {
        LOG(ERROR,"fd is invalid:%d",fd);
        return -1;
    }
    // buf must not too smaller then 2 bytes,otherwise it will cause recv return -1 forever
    char buf[14460] = {0};
    int retLen     = 0;

    t_msg *newmsg = NewMsg(sizeof(buf));

    for(;;)
    {
        retLen = recv(fd,buf,sizeof(buf),MSG_DONTWAIT);
        if(0 == retLen)
        {
            //客户端主动断开
            LOG(INFO,"Client fd[%d]close socket\n",fd);
            break;
        }
        else if(-1 == retLen)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK||errno == EINTR)
            {
                usleep(1000);
                LOG(INFO,"recv on fd[%d] return -1,try recv msg again.",fd);
                //continue;
                break;
            }
            else if(errno == ECONNRESET)
            {
                LOG(INFO,"Client fd[%d]reset socket\n",fd);
                break;
            }
            else
            {
                LOG(ERROR,"recv fd[%d] return failed:%s\n",fd ,strerror(errno));
                break;
            }
        }
        else if(retLen < sizeof(buf))
        {
            AttachBuff2Msg(&newmsg,buf,retLen);
            break;
        }
        else
        {
            AttachBuff2Msg(&newmsg,buf,retLen);
        }
        //LOG(INFO,"msg:[%s] len=%d,free=%d,newmsglen=%d\n",newmsg->buff,retLen,newmsg->free,newmsg->len);
    }
    
    LOG(INFO,"msg from fd[%d]:[%s] ,total len=%d\n",fd,newmsg->buff,newmsg->len);
    //交由业务进行处理:thred
    //FreeMsg(newmsg);
    return newmsg->len? newmsg->len:retLen;
}
int CNet::OnWrite(int fd)
{
    if(fd < 0)
    {
        LOG(ERROR,"fd is invalid:%d",fd);
        return -1;
    }
    int sendLen = 0;
    char *pBody = "ret=0";
    char pMsg[1024] = {0};
    snprintf(pMsg,sizeof(pMsg),"HTTP/1.1 200 OK\r\n Connection: close\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\nret=0\r\n",strlen(pBody)+1);
    int  totalLen = strlen(pMsg)+1;
    int totalSendLen = 0;
    while(1)
    {
        LOG(DEBUG,"Try to send\n");
        sendLen = send(fd,pMsg,totalLen,0);
        if(-1 == sendLen)
        {
            LOG(ERROR,"Send to client fd[%d] failed:%s\n",fd,strerror(errno));
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                usleep(1000);   
                continue;
            }
            else 
            { 
                LOG(ERROR,"send return failed:%d\n",strerror(errno));
                return -1; 
            }
        }
        totalSendLen += sendLen;
        if(totalLen <= totalSendLen)
        {
            LOG(DEBUG,"Send response ok:%d\n",totalSendLen);
            return sendLen;
        }
    }
}
/**
 *讲处理函数与fd绑定
 */
int CNet::AttachHandler(int fd,void*(*func)(void*))
{
}


/**
 * 将数据分发到后端，提供给后端线程处理
 */
int CNet::Dispatch2BackQueue()
{

}

int CNet::GetFdFromEpollData(struct EpollData* pEpollData)
{
    if(pEpollData == NULL)
    {
        return -1;
    }
    return pEpollData->fd;
}

struct EpollData* CNet::CreateEpollData(int fd,int (CNet::*pFunc)(int))
{
    struct EpollData *pData = NULL ;
    pData = (struct EpollData*)malloc(sizeof(EpollData));
    pData->wFunc = pFunc;
    pData->rFunc = pFunc;
    pData->fd    = fd;
    return pData;
}

