#include "threadpool.h"
void* msg_handler(void* pArgs);

CThreadPool::CThreadPool(CBaseProcess *pProcessObj, HANDLER_TYPE type ,unsigned int poolSize /*=10*/,char* threadName/*="[empty]"*/):
    m_pProcessObj(pProcessObj),m_poolSize(poolSize),m_strThreadName(threadName),m_handlerType(type)
{
    this->InitThreadPool();
}

int CThreadPool::InitThreadPool()
{
    if(m_pProcessObj == NULL)
    {
        LOG(ERROR,"init thread pool failed: args 1 is invalid must not NULL");
        return -1;
    }
    if(m_poolSize <= 0)
    {
        LOG(ERROR,"init thread pool failed: args 2 is invalid must >0");
        return -2;
    }
    pthread_t thread = 0;
    int ret = 0;
    for(int i  = 0;i < m_poolSize;i++)
    {
        ret = pthread_create(&thread,NULL,msg_handler,(void*)this);
        if(ret == 0)
        {
            m_threadList.push_back(thread);
        }
        else
        {
            LOG(ERROR,"[%s]-create thread failed:%s",m_strThreadName.c_str());
        }
    }
    return m_threadList.size();
}

CMsg* CThreadPool::PopBackFromList()
{
    m_mutexMsgList.Lock();
    CMsg* pMsg = NULL;
    if(m_msgList.size())
    {
        pMsg = m_msgList.back();
        m_msgList.pop_back();
    }
    m_mutexMsgList.UnLock();
    return pMsg;
}
CMsg* CThreadPool::PopFrontFromList()
{
    m_mutexMsgList.Lock();
    CMsg* pMsg = NULL;
    if(m_msgList.size())
    {
        pMsg = m_msgList.front();
        m_msgList.pop_front();
    }
    m_mutexMsgList.UnLock();
    return pMsg;
}
void CThreadPool::FreeCMsg(CMsg*pMsg)
{
    if(pMsg)
    {
        pMsg->pFreeMsgFunc(pMsg->msg);
        free(pMsg);
    }
}
int CThreadPool::PushMsgToList(CMsg *msg)
{
    if(msg)
    {
        m_mutexMsgList.Lock();
        m_msgList.push_back(msg);
        m_mutexMsgList.UnLock();
    }

}
void* msg_handler(void* pArgs)
{
    CThreadPool* pThis = (CThreadPool*)pArgs;

    if(pThis == NULL)
    {
        int exit_code = -1;
        pthread_exit(&exit_code);
    }
    for(;;)
    {
        pThis->m_mutexMsgList.Lock();
        CMsg* pMsg = pThis->PopBackFromList();
        pThis->m_mutexMsgList.UnLock();

        if(pMsg)
        {
            LOG(DEBUG,"recv msg:%s\n",pMsg->msg);
            if(pThis->m_handlerType == EREQUEST)
                pThis->m_pProcessObj->ProcessReqMsg(pMsg);
            else if(pThis->m_handlerType == ERESPONSE)
                pThis->m_pProcessObj->ProcessRespMsg(pMsg);
            pThis->FreeCMsg(pMsg);
        }
        // <todo> reload
    }
}

