#pragma once
#include "msghandler/msg.h"
class CBaseProcess
{
    public:
        CBaseProcess();
        virtual ~CBaseProcess();
        virtual void ProcessMsg(void* pMsg) = 0;
};

