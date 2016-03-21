#include "msghandler/base_process.h"
#include "common/global.h"
class CSimpleProcess:public  CBaseProcess
{
    public:
        virtual void ProcessMsg(void* pMsg);
};
