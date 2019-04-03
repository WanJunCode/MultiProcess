#include "../AsyncLogging.h"
#include "../Logging.h"
#include "../Timestamp.h"

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

// 500 m
off_t kRollSize = 500*1000*1000;

WJ::AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
  g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
  // 设置输出函数
  WJ::Logger::setOutput(asyncOutput);

  int cnt = 0;
  const int kBatch = 10000;
  WJ::string empty = " ";
  WJ::string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 30; ++t)
  {
    WJ::Timestamp start = WJ::Timestamp::now();
    // 写入 kBatch 数量的日志
    for (int i = 0; i < kBatch; ++i)
    {
      LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
               << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    WJ::Timestamp end = WJ::Timestamp::now();
    printf("%f\n", timeDifference(end, start)*1000000/kBatch);
    // struct timespec ts = { 0, 500*1000*1000 };
    // nanosleep(&ts, NULL);
  }
}

int main(int argc, char* argv[])
{
  {
    // set max virtual memory to 2GB. 设置最大的虚拟内存为 2GB
    size_t kOneGB = 1000*1024*1024;
    rlimit rl = { 2*kOneGB, 2*kOneGB };
    setrlimit(RLIMIT_AS, &rl);
  }

  printf("pid = %d\n", getpid());

  char name[256] = { 0 };
  strncpy(name, argv[0], sizeof name - 1);
  printf("name [%s]\n",name);
  WJ::AsyncLogging log(::basename(name), kRollSize);
  log.start();
  g_asyncLog = &log;

  bool longLog = argc > 1;
  bench(longLog);
}
