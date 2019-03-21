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

extern void client_to_server();
extern void test_for_concurrence(const char *ip,  size_t port, size_t num);

int main(int argc, char *argv[])
{
    if(argc<=2){
        printf("必须输入两个参数\n");
        return -1;
    }
    test_for_concurrence(argv[1], 12345, atoi(argv[2]));

    // client_to_server();

    // u_char buffer[2048];
    // memset(buffer,0,sizeof(buffer));
    // const char *msg = "hello world";
    // // 将后续的 fmt 和 参数 合成字符串放入 buffer 字符数组中
    // ngx_snprintf(buffer, 2048, "nginx test [%d] [%s]",12, msg);
    // printf("[%s]\n",buffer);

    return 0;
}
