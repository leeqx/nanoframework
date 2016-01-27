#pragma once
#include <stdlib.h>
#include <string.h>

#include "common/global.h"
typedef struct msg
{
    int len;
    int free;
    char *buff;
}t_msg;

t_msg* NewMsg(int size)
{
    if(size < 0)
        return NULL;
    t_msg *newmsg =(t_msg*) malloc(sizeof(t_msg));
    ASSERT_EQ(newmsg,NULL);

    newmsg->buff = (char*)malloc(size);
    newmsg->free = size;
    newmsg->len  = 0;

    ASSERT_EQ(newmsg->buff,NULL);
    return newmsg;
}

void FreeMsg(t_msg *msg)
{
    if (NULL != msg)
    {
        if(NULL != msg->buff)
        {
            free(msg->buff);
            msg->buff = NULL;
        }
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

    t_msg　*newmsg = NewMsg(newsize);
    if(NULL != newmsg)
    {
        memcpy(newmsg,srcmsg,sizeof(t_msg));
        newmsg->free = newsize - newmsg->len;
        FreeMsg(srcmsg);
    }
    return newmsg;
}

t_msg* AttchBuff2Msg(t_msg**srcmsg,char* buff,int len)
{
    if((*srcmsg)->free < len)
    {
        *srcmsg = ReNewMsg(*srcmsg,(*srcmsg)->free+(*srcmsg)->len+len);
        memcpy((*srcmsg)->buff + (*srcmsg)->len,buff,len);
    }
    else
    {
        memcpy((*srcmsg)->buff + (*srcmsg)->len,buff,len);
    }

    return *srcmsg;
}
