#include "msghandler/simple_process.h"
#include "common/global.h"
#include "handler_factory.h"

void CSimpleProcess::ProcessReqMsg(void* pMsg)
{
    CMsg *pTmpMsg = (CMsg*)pMsg;
    LOG(DEBUG,"recv from client:%d msg:%s\n",pTmpMsg->socketFd,pTmpMsg->msg);
    //<TODO> according message type to handler msg
    CBaseHandler *pHandler =CNanoFactory::Instance()->CreateHandleByType((t_msg*)(pTmpMsg->msg));

}
void CSimpleProcess::ProcessRespMsg(void* pMsg)
{
}
