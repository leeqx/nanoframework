#include <stdio.h>
#include <unistd.h>
#include "net/net.h"
int main(int argc,char* argv[])
{

    //load config
    //
    // init server
    CNet net(123);
    if(0 == net.initSocket(9999,123))
        net.EpollWait();
    return 0;
}
