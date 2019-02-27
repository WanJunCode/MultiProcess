#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>     // bzero

#include "../common/net/net_tool.h"
#include "MultiProcess.h"
#include "shm_mutex.h"

shm_mutex_t *process_mutex = (shm_mutex_t *)mmap(NULL, sizeof(shm_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
// shm_mutex_t process_mutex;

int listenfd;

void child_process(int num){
    printf("child process [%d] begin\n",num);
    if(ngx_shmtx_trylock(process_mutex)){
        printf("pid [%d] get process mutex\n",getpid());
        sleep(2);
        ngx_shmtx_unlock(process_mutex);
    }else{
        fprintf(stdout,"pid [%d] can't get process mutex\n",getpid());
    }
    sleep(180);
    printf("child process [%d] end\n",num);
}

void parentControl(MultiProcess &mgr){
    while(true){
        std::string cmd;
        std::cin>>cmd;

        if(std::string::npos != cmd.find("over")){
            mgr.killChildren();
            printf("break\n");
            break;
        }else if(std::string::npos != cmd.find("reload")){
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

void mmap_test(){
    int *ptr;
    int i=1;

    ptr = (int *)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);

    *ptr = 1;

    if(fork() == 0){
        printf("\n ptrAddress=%p valuesAddress=%p values=%d \n",&ptr,ptr,*ptr);
        printf("\n i=%d i_address=%p \n",i,&i);
        *ptr = 2;
        i = 2;
        sleep(2);
        exit(0);
    }
    sleep(1);
    printf("\n ptrAddress=%p valuesAddress=%p values=%d \n",&ptr,ptr,*ptr);
    printf("\n i=%d i_address=%p \n",i,&i);
    sleep(2);    

    // fork 后父子进程所有变量地址都是相同的 逻辑地址，通过 mmu 转化为实际的物理地址

    printf("parent process end\n");
}

int main1(int argc,char* argv[]){

    printf("argc = %d\n",argc);
    if(argc<2){
        printf("usage\n");
        return -1;
    }

    // auto pid = getpid();
    // auto ppid = getppid();
    // printf("pid = [%d] , ppid = [%d]\n",pid,ppid);

    MultiProcess &mgr = MultiProcess::getInstance();
    mgr.chkServerRunWpid();

    int size = atoi(argv[1]);
    for(int i=0; i<size ;++i){
        mgr.fork(child_process,i);
    }

    parentControl(mgr);
    return 0;
}


#include <thread>
using namespace NET;

// 对应 libevent server 中 echo package 包格式 
typedef struct message{
    uint8_t     identity;       // 0x7E
    uint16_t    length;
}__attribute__((packed)) echo_message_t;

bool loop = false;

void receive_loop(remote_client client, int bufferSize){
    char buffer[bufferSize];
    bzero(buffer,bufferSize);
    while(loop){
        // 阻塞式
        auto length = recv(client.clientfd,buffer,bufferSize,0);
        printf("receive :[%s]\n",buffer);
        bzero(buffer,bufferSize);
        if(strncmp(buffer,"byebye",strlen("byebye")))
            break;
    }
}

void client_to_server(char *argv[]){

    auto client = Connect("127.0.0.1",atoi(argv[1]));
    if(client.clientfd){
        printf("连接成功\n");
    }else{
        printf("something wrong : 连接不成功\n");
        return;
    }

    std::thread receive(receive_loop, std::ref(client),60);
    loop = true;
    while(true){
        std::string message;
        std::cin>>message;
        if(message.find("return")!=std::string::npos){
            loop = false;
            const char *will = "byebye";
            send(client.clientfd,will,strlen(will),0);
            break;
        }else if(message.find("send")!= std::string::npos){
            std::string sendMessage;
            const char *array = "this is wanjun call";
            echo_message_t header{ 0x7E, htons( strlen(array) ) };
            sendMessage.append((char *)(&header),sizeof(header));
            sendMessage.append(array,strlen(array));
            write(client.clientfd,sendMessage.data(),sendMessage.length());
            continue;
        }
        std::string sendMessage;
        echo_message_t header{ 0x7E, htons((uint16_t)message.length()) };
        sendMessage.append((char *)(&header),sizeof(header));
        sendMessage.append(message.data(),message.length());
        write(client.clientfd,sendMessage.data(),sendMessage.length());
    }
    receive.join();

    Close(client);
    return;
}

int main(int argc,char* argv[]){
    client_to_server(argv);
    return 0;
}
