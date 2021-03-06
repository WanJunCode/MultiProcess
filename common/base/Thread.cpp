// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "Thread.h"

#include "CurrentThread.h"
#include "Exception.h"
#include "Logging.h"

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace WJ
{
namespace detail
{

// 获得线程id
pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
  // 给 fork 后的进程设置主线程名称
  WJ::CurrentThread::t_cachedTid = 0;
  WJ::CurrentThread::t_threadName = "main";
  CurrentThread::tid();
  // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

// 线程名称 初始器
class ThreadNameInitializer
{
 public:
  ThreadNameInitializer()
  {
    WJ::CurrentThread::t_threadName = "main";
    // 缓存线程id
    CurrentThread::tid();
    // 设置线程线程 fork 后需要调用的函数
    pthread_atfork(NULL, NULL, &afterFork);
  }
};

// 定义一个全局变量 线程名称 初始器
ThreadNameInitializer init;

// 线程数据
struct ThreadData
{
  typedef WJ::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;         // 线程执行函数
  string name_;             // 线程名称
  pid_t* tid_;              // 线程 id
  CountDownLatch* latch_;   // 用于指明线程是否 执行完成

  ThreadData(ThreadFunc func,
             const string& name,
             pid_t* tid,
             CountDownLatch* latch)
    : func_(std::move(func)),
      name_(name),
      tid_(tid),
      latch_(latch)
  { }

  // 在线程中执行的函数
  void runInThread()
  {
    // 获得当前线程id
    *tid_ = WJ::CurrentThread::tid();
    tid_ = NULL;
    latch_->countDown();
    latch_ = NULL;

    // 赋值线程名称
    WJ::CurrentThread::t_threadName = name_.empty() ? "WJThread" : name_.c_str();
    ::prctl(PR_SET_NAME, WJ::CurrentThread::t_threadName);
    try
    {
      // 执行该函数
      func_();
      // 设置当前线程名称为 finished
      WJ::CurrentThread::t_threadName = "finished";
    }
    catch (const Exception& ex)
    {
      WJ::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
      abort();
    }
    catch (const std::exception& ex)
    {
      WJ::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      abort();
    }
    catch (...)
    {
      WJ::CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
      throw; // rethrow
    }
  }
};//struct ThreadData

// 开始执行的线程函数
void* startThread(void* obj)
{
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

}  // namespace detail


// =============================  CurrentThread  ==============================
// 缓存 线程id
void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = detail::gettid();
    // 
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

// 判断是否主线程id
bool CurrentThread::isMainThread()
{
  // 线程id 和 进程id 的比较
  return tid() == ::getpid();
}

// 1000000 表示 1 秒
void CurrentThread::sleepUsec(int64_t usec)
{
  struct timespec ts = { 0, 0 };
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, NULL);
}

// =============================  Thread  ==============================
// 定义 原子变量 用于记录创建了多少线程
AtomicInt32 Thread::numCreated_;

Thread::Thread(ThreadFunc func, const string& n)
  : started_(false),
    joined_(false),     // 默认是不需要等待线程结束
    pthreadId_(0),
    tid_(0),
    func_(std::move(func)),
    name_(n),
    latch_(1)
{
  setDefaultName();
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}

// 使用 原子变量 设置 Thread id
void Thread::setDefaultName()
{
  int num = numCreated_.incrementAndGet();
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::start()
{
  assert(!started_);
  started_ = true;
  // 创建一个 ThreadData 最为 obj 传入 线程函数中
  // FIXME: move(func_)
  detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
  {
    // 线程创建失败
    started_ = false;
    delete data; // or no delete?
    LOG_SYSFATAL << "Failed in pthread_create";
  }
  else
  {
    // 在此等待 线程函数执行完毕
    latch_.wait();
    assert(tid_ > 0);
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  // 等待函数执行完毕
  return pthread_join(pthreadId_, NULL);
}

}  // namespace WJ
