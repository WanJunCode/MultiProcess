#include <stdio.h>
#include "MultiProcess.h"

int main(int argc,char** argv){
    MultiProcess &mgr = MultiProcess::getInstance();
    mgr.chkServerRunWpid();
    return 0;
}