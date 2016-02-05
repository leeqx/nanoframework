#pragma once
#include <stdlib.h>
#include <string.h>

#include "common/global.h"
typedef struct msg
{
    int len;
    int free;
    char buff[0];
}t_msg;

t_msg* NewMsg(int size);
void FreeMsg(t_msg *msg);
t_msg* DupMsg(t_msg *srcmsg,t_msg *newmsg);
t_msg* ReNewMsg(t_msg *srcmsg,int newsize);
t_msg* AttachBuff2Msg(t_msg**srcmsg,char* buff,int len);

