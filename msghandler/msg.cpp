#include "msghandler/msg.h"

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

    t_msg *newmsg = NewMsg(newsize);
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
