#pragma once
#include "msghandler/msg.h"
class CBaseProcess
{
    public:
        CBaseProcess();
        virtual ~CBaseProcess();
        virtual void ProcessReqMsg(void* pMsg) = 0;
        virtual void ProcessRespMsg(void* pMsg) = 0;
};

