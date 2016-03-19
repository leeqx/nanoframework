#pragma once
#include <pthread.h>
#include <deque>
#include <string>
#include <vector>
#include "msghandler/base_process.h"
#include "common/global.h"
#include "mutex.h"

using namespace std;

typedef struct _CMsg
{
    int socketFd;
    // 该消息内容长度
    int msgLength;
    // 消息内容
    void *msg;
    //释放消息内存的函数
    void (*pFreeMsgFunc)(void* msg);
    void* ptr;
}CMsg;


class CThreadPool
{
    public:
        CThreadPool(CBaseProcess *pProcessObj,unsigned int poolSize=8,char* threadName="[empty]");
        ~CThreadPool();
        int InitThreadPool();
        int PushMsgToList(CMsg *msg);
        CMsg* PopBackFromList();
        CMsg* PopFrontFromList();
        void FreeCMsg(CMsg *pMsg);
        friend void* msg_handler(void* pArgs);
    private:
        typedef deque<CMsg*>   THREAD_ARGS;
        typedef vector<pthread_t> THREADLIST;
        unsigned int m_poolSize;
        //msgList 队列
        CMutex       m_mutexMsgList;
        string       m_strThreadName;
        THREAD_ARGS  m_msgList;
        //线程列表，存放每个线程的ID
        THREADLIST   m_threadList;
        CBaseProcess *m_pProcessObj;

        typedef void* (CThreadPool::*pHandlerFunc)(void*);
};

