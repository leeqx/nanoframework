#include <stdio.h>
#include <unistd.h>
#include "net/net.h"
#include "msghandler/simple_process.h"

int main(int argc,char* argv[])
{

    //load config
    //
    // init server
    CBaseProcess *pSimpleProc = new CSimpleProcess; 
    CNet net(123,pSimpleProc);
    if(0 == net.initSocket(9999,123))
        net.EpollWait();

    delete pSimpleProc;
    return 0;
}
