#include "../CurrentThread.h"
#include "../Mutex.h"
#include "../Thread.h"
#include "../Timestamp.h"

#include <map>
#include <string>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

WJ::MutexLock g_mutex;
std::map<int, int> g_delays;

void threadFunc()
{
  //printf("tid=%d\n", WJ::CurrentThread::tid());
}

void threadFunc2(WJ::Timestamp start)
{
  WJ::Timestamp now(WJ::Timestamp::now());
  int delay = static_cast<int>(timeDifference(now, start) * 1000000);
  WJ::MutexLockGuard lock(g_mutex);
  ++g_delays[delay];
}

void forkBench()
{
  sleep(10);
  WJ::Timestamp start(WJ::Timestamp::now());
  int kProcesses = 10*1000;

  for (int i = 0; i < kProcesses; ++i)
  {
    pid_t child = fork();
    if (child == 0)
    {
      exit(0);
    }
    else
    {
      waitpid(child, NULL, 0);
    }
  }

  double timeUsed = timeDifference(WJ::Timestamp::now(), start);
  printf("process creation time used %f us\n", timeUsed*1000000/kProcesses);
  printf("number of created processes %d\n", kProcesses);
}

int main(int argc, char* argv[])
{
  printf("pid=%d, tid=%d\n", ::getpid(), WJ::CurrentThread::tid());
  WJ::Timestamp start(WJ::Timestamp::now());

  int kThreads = 100*1000;
  for (int i = 0; i < kThreads; ++i)
  {
    WJ::Thread t1(threadFunc);
    t1.start();
    t1.join();
  }

  double timeUsed = timeDifference(WJ::Timestamp::now(), start);
  printf("thread creation time %f us\n", timeUsed*1000000/kThreads);
  printf("number of created threads %d\n", WJ::Thread::numCreated());

  for (int i = 0; i < kThreads; ++i)
  {
    WJ::Timestamp now(WJ::Timestamp::now());
    WJ::Thread t2(std::bind(threadFunc2, now));
    t2.start();
    t2.join();
  }
  {
    WJ::MutexLockGuard lock(g_mutex);
    for (const auto& delay : g_delays)
    {
      printf("delay = %d, count = %d\n",
             delay.first, delay.second);
    }
  }

  forkBench();
}
