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
#include <pthread.h>

#include "shm_mutex.h"

#include "../common/nginx/nginx_log.h"
#include "../common/net/net_tool.h"

#include <thread>
using namespace NET;

int listenfd;

// 对应 libevent server 中 echo package 包格式
typedef struct message
{
    uint8_t identity; // 0x7E
    uint16_t length;
} __attribute__((packed)) echo_message_t;

size_t bufferSize = 1024;

void *receive_loop(void *args)
{
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
        // if (0 == strncmp(buffer, "return", strlen("return")))
        //     break;
    }
    printf("out of receive loop\n");
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

    // std::thread receive(receive_loop, std::ref(client),60);
    pthread_t receiveThread;
    pthread_create(&receiveThread,NULL,receive_loop,&client);
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
    
    pthread_cancel(receiveThread);
    pthread_join(receiveThread,NULL);
    printf("after thread join\n");
    Close(client);
    return;
}