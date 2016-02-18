#include "msghandler/msg.h"
#include "common/global.h"

t_msg* NewMsg(int size)
{
    if(size < 0)
        return NULL;
    t_msg *newmsg =(t_msg*) malloc(sizeof(t_msg)+size);
    ASSERT_EQ(newmsg,NULL);

    newmsg->free = size;
    newmsg->len  = 0;

    ASSERT_EQ(newmsg->buff,NULL);
    return newmsg;
}

void FreeMsg(t_msg *msg)
{
    if (NULL != msg)
    {
        free(msg);
        msg = NULL;
    }
}
t_msg* DupMsg(t_msg *srcmsg,t_msg *newmsg)
{
    if( NULL == newmsg && NULL != srcmsg)
    {
        newmsg = NewMsg(srcmsg->len + srcmsg->free);
        memcpy(newmsg,srcmsg,sizeof(srcmsg));
    }
    return newmsg;
}

t_msg* ReNewMsg(t_msg *srcmsg,int newsize)
{
    if(NULL == srcmsg)
    {
        return NULL; 
    }

    LOG(DEBUG,"before:%p %p\n",srcmsg,(int*)malloc(1));
    t_msg *newmsg = NewMsg(newsize);
    LOG(DEBUG,"after:%p\n",newmsg);
    if(NULL != newmsg)
    {
        memcpy(newmsg,srcmsg,sizeof(t_msg));
        memcpy(newmsg->buff,srcmsg->buff,srcmsg->len);
        newmsg->free = newsize - newmsg->len;
        FreeMsg(srcmsg);
    }
    return newmsg;
}

t_msg* AttachBuff2Msg(t_msg**srcmsg,char* buff,int len)
{
    if((*srcmsg)->free < len)
    {
        *srcmsg = ReNewMsg(*srcmsg,(*srcmsg)->free+(*srcmsg)->len+len*2);
        memcpy((*srcmsg)->buff + (*srcmsg)->len,buff,len);
        (*srcmsg)->len += len;
    }
    else
    {
        memcpy((*srcmsg)->buff + (*srcmsg)->len,buff,len);
        (*srcmsg)->len += len;
    }

    return *srcmsg;
}
//===========================msg_list=================================
t_msg_list* CreateMsgList()
{
    t_msg_list * pMsgList = (t_msg_list*)malloc(sizeof(t_msg_list));
    if(pMsgList != NULL)
    {
        pMsgList->node.msg = NULL; //header node
        pMsgList->node.fd   = -1;
        pMsgList->node.pHandler = NULL;
        pMsgList->next = NULL; 
    }
    return pMsgList;
}
void FreeMsgList(t_msg_list *pOrgMsg)
{
    if(pOrgMsg != NULL)
    {
        if(pOrgMsg->node != NULL && pOrgMsg->node->pFreeNodeFunc) 
        {
            pOrgMsg->pFreeNodeFunc(pOrgMsg->node);
        }
        pOrgMsg->next = NULL;
        free(pOrgMsg);
        pOrgMsg = NULL;
    }
}
t_msg_list* AddMsgToMsgList(t_msg_list *pOrgMsg,t_msg *pMsg,int fd,void (*pFreeNodeFunc)(void* ptr))
{
    if(NULL == pOrgMsg)
    {
        return NULL; 
    }
    t_msg_list *pNewMsgNode = (t_msg_list*)malloc(sizeof(t_msg_list));
    if(NULL == pNewMsgNode)
    {
        return pOrgMsg; 
    }
    pNewMsgNode->node->pFreeNodeFunc= pFreeNodeFunc? pFreeNodeFunc:free;
    pNewMsgNode->node->pHandler = ;
    pNewmsgNode->node->fd = fd;
    pNewMsgNode->next = NULL;

}
t_msg_node* PopTailMsglist(t_msg_list *pOrgMsg)
{
    t_msg_list *pHead = pOrgMsg;
    
    while(pHead->next && pHead->next->next)
    {
        pHeader = pHead->next;
    }
    t_msg_list *pTail = pHead->next;
    pHeader->next = NULL;
    return pTail->node;
}
t_msg_node* PopHeadMsglist(t_msg_list *pOrgMsg)
{
    t_msg_list *pHead = pOrgMsg;
    pOrgMsg = pOrgMsg->next;
    return pHead->node;
}

