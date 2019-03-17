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
#include <thread>
#include <pthread.h>

#include "shm_mutex.h"
#include "../common/nginx/nginx_log.h"
#include "../common/net/net_tool.h"

using namespace NET;

// 对应 libevent server 中 echo package 包格式
typedef struct message
{
    uint8_t identity; // 0x7E
    uint16_t length;
} __attribute__((packed)) echo_message_t;

size_t bufferSize = 1024;

void cleanup_method(void *args){
    printf("this is receive loop thread's cleanup mothed\n");
}

void *async_receive_thread(void *args)
{
    pthread_cleanup_push(cleanup_method,NULL);
    remote_client *client = (remote_client *)(args);
    char buffer[bufferSize];
    bzero(buffer, bufferSize);
    while (true)
    {
        // 阻塞式
        printf("before receive\n");
        auto length = recv(client->clientfd, buffer, bufferSize, 0);
        printf("receive[%lu] :[%s]\n", length, buffer);
        bzero(buffer, bufferSize);
    }
    // never be here
    printf("out of receive loop\n");
    pthread_cleanup_pop(1);
    return NULL;
}

void client_to_server()
{
    auto client = Connect("127.0.0.1", 12345);
    if (client.clientfd)
    {
        printf("连接成功\n");
    }
    else
    {
        printf("something wrong : 连接不成功\n");
        return;
    }

    pthread_t receiveThread;
    pthread_create(&receiveThread,NULL,async_receive_thread,&client);
    while (true)
    {
        std::string message;
        std::cin >> message;
        if (message.find("return") != std::string::npos)
        {
            const char *will = "return";
            send(client.clientfd, will, strlen(will), 0);
            break;
        }
        else if (message.find("send") != std::string::npos)
        {
            std::string sendMessage;
            const char *array = "this is wanjun call";
            echo_message_t header{0x7E, htons(strlen(array))};
            sendMessage.append((char *)(&header), sizeof(header));
            sendMessage.append(array, strlen(array));
            write(client.clientfd, sendMessage.data(), sendMessage.length());
            continue;
        }
        srand(time(NULL));
        printf("rand [%d]\n", rand());
        std::string sendMessage;
        echo_message_t header{0x7E, htons((uint16_t)message.length())};
        sendMessage.append((char *)(&header), sizeof(header));
        sendMessage.append(message.data(), message.length());
        write(client.clientfd, sendMessage.data(), sendMessage.length());
    }
    
    // 使用 pthread_cancel 是会调用 pthread_cleanup mothed 的
    pthread_cancel(receiveThread);
    pthread_join(receiveThread,NULL);
    printf("after thread join\n");
    Close(client);
    return;
}

void test_for_concurrence(const char *ip,  size_t port, size_t num)
{
    printf("num [%lu] , port [%lu]\n", num, port);
    std::vector<remote_client> clientVec;
    clientVec.reserve(num);
    size_t volume = 0;
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