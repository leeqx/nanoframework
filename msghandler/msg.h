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
t_msg* DupMsg(t_msg *srcmsg);
t_msg* ReNewMsg(t_msg *srcmsg,int newsize);
t_msg* AttachBuff2Msg(t_msg**srcmsg,char* buff,int len);

//===========================msg_list=================================
typedef struct msg_node
{
    t_msg *data;
    int   fd;
    //free node
    void (*pFreeNodeFunc)(void* ptr);
    // no use
    void* (*pHandler)(int*);
}t_msg_node;

typedef struct msg_list
{
    t_msg_node node;
    struct msg_list* next;
}t_msg_list;


t_msg_list* CreateMsgList();
t_msg_list* AddMsgToMsgList(t_msg_list *pOrgMsg,t_msg *pMsg,int fd,void(*pFreeNodeFunc)(void* ptr));
t_msg_node* PopTailMsglist(t_msg_list *pOrgMsg);
t_msg_node* PopHeadMsglist(t_msg_list *pOrgMsg);
void FreeMsgList(t_msg_list *pOrgMsg);

