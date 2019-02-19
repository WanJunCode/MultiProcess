#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>
#include <iostream>

#include "MultiProcess.h"

void test(){
    pid_t pid;

    pid = fork();
    if(pid<0){ /* 如果出错 */
        printf("error occurred!\n");
    }else if(pid==0){ /* 如果是子进程 */

        exit(0);
        // 子进程 exit 后会进入zombie 僵尸状态
    } else {/* 如果是父进程 */
        sleep(60); /* 休眠60秒，这段时间里，父进程什么也干不了 */
        printf("parent test====================\n");
        wait(NULL); /* 收集僵尸进程 */
    }
}

void child_process(int num){
    printf("child process [%d] begin\n",num);
    sleep(180);
    printf("child process [%d] end\n",num);
}

void parentControl(MultiProcess &mgr){
    while(true){
        std::string cmd;
        std::cin>>cmd;

        if(std::string::npos != cmd.find("return")){
            mgr.killChildren();
            printf("break\n");
            break;
        }else if(std::string::npos != cmd.find("check")){
            printf("check child\n");
            // 重新启动 子进程
            mgr.checkChildren();
        }else if(std::string::npos != cmd.find("killall")){
            printf("kill all child\n");            
            mgr.killChildren();
        }else if(std::string::npos != cmd.find("list")){
            mgr.listPid();
        }else if(std::string::npos != cmd.find("kill")){
            int pid;
            std::cin>>pid;
            mgr.killPid(pid);
        }else if(std::string::npos != cmd.find("add")){
            mgr.fork(child_process,mgr.getSize());
        }
    }
}

int main(int argc,char* argv[]){
    printf("argc = %d\n",argc);
    if(argc<2){
        printf("usage\n");
        return -1;
    }

    MultiProcess &mgr = MultiProcess::getInstance();
    mgr.chkServerRunWpid();

    int size = atoi(argv[1]);
    for(int i=0; i<size ;++i){
        mgr.fork(child_process,i);
    }

    parentControl(mgr);
    
    printf("parent process end\n");
    return 0;
}