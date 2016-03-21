#include "msghandler/simple_process.h"
#include "common/global.h"

void CSimpleProcess::ProcessMsg(void* pMsg)
{
    CMsg *pTmpMsg = (CMsg*)pMsg;
    LOG(DEBUG,"recv from client:%d msg:%s\n",pTmpMsg->socketFd,pTmpMsg->msg);
}
