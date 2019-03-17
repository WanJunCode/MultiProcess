#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h> // bzero
#include <stdlib.h>

#include "shm_mutex.h"

#include "../common/nginx/nginx_log.h"
#include "../common/net/net_tool.h"

#include <thread>
using namespace NET;

// shm_mutex_t process_mutex;



void test_for_concurrence(const char *ip, size_t num, size_t port)
{
    printf("num [%d] , port [%d]\n", num, port);
    std::vector<remote_client> clientVec;
    clientVec.reserve(num);
    static size_t volume = 0;

    int failConnection = 0;
    for (size_t i = 0; i < num; ++i)
    {
        auto conn = Connect(ip, port);
        clientVec.push_back(conn);
        if (conn.clientfd)
        {
            volume++;
            if (volume > 100)
            {
                printf("当前连接数量 num = [%d]\n", i);
                volume = 0;
            }
        }
        else
        {
            ++failConnection;
            printf("something wrong : 连接不成功\n");
        }
    }

    printf("连接完成: 失败的连接数量为 [%d]\n", failConnection);
    std::string cmd;
    std::string msg = "message from client";
    while (true)
    {
        std::cin >> cmd;
        if (0 == strncmp(cmd.data(), "over", strlen("over")))
        {
            printf("退出...\n");
            break;
        }
        else if(0 == strncmp(cmd.data(),"concurr",strlen("concurr")))
        {
            size_t conncurr = 0;
            std::cout<<"请输入需要并发的数量:";
            std::cin>>conncurr;
            if(conncurr>num){
                continue;
            }
            for(size_t i=0;i<conncurr;++i){
                printf("send msg\n");
                write(clientVec[i].clientfd, msg.data(), msg.length());
            }
            continue;
        }

        try
        {
            size_t selectClient = atoi(cmd.data());
            printf("select Cliet [%lu] clientVec.size() = [%lu]\n", selectClient, clientVec.size());
            if (selectClient < clientVec.size())
            {
                printf("send msg\n");
                write(clientVec[selectClient].clientfd, msg.data(), msg.length());
            }
        }
        catch (...)
        {
            printf("atoi fail\n");
        }
    }

    for (size_t i = 0; i < clientVec.size(); ++i)
    {
        Close(clientVec[i]);
    }
    printf("test for concurrency end\n");
}

extern void client_to_server();

int main(int argc, char *argv[])
{
    // if(argc<=2){
    //     printf("必须输入两个参数\n");
    //     return 1;
    // }

    // client_to_server(argv);
    // test_for_concurrence(argv[1], atoi(argv[2]), 12345);

    client_to_server();

    // u_char buffer[2048];
    // memset(buffer,0,sizeof(buffer));
    // const char *msg = "hello world";
    // // 将后续的 fmt 和 参数 合成字符串放入 buffer 字符数组中
    // ngx_snprintf(buffer, 2048, "nginx test [%d] [%s]",12, msg);
    // printf("[%s]\n",buffer);

    return 0;
}
