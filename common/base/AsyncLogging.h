// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef WJ_BASE_ASYNCLOGGING_H
#define WJ_BASE_ASYNCLOGGING_H

#include "BlockingQueue.h"
#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"

#include <atomic>
#include <vector>

namespace WJ
{

// 异步日志
class AsyncLogging : noncopyable
{
public:
  AsyncLogging(const string &basename,
               off_t rollSize, // 滚动大小
               int flushInterval = 3);

  ~AsyncLogging()
  {
    if (running_)
    {
      stop();
    }
  }

  void append(const char *logline, int len);

  void start()
  {
    running_ = true;
    // 开启线程
    thread_.start();
    // 必须等待写入线程开启
    latch_.wait();
  }

  // 使用了 clang NO_THREAD_SAFETY_ANALYSIS 属性，作用于 method ； 关闭线程的安全性检测
  void stop() NO_THREAD_SAFETY_ANALYSIS
  {
    running_ = false;
    cond_.notify();
    thread_.join();
  }

private:
  void threadFunc();

  typedef WJ::detail::FixedBuffer<WJ::detail::kLargeBuffer> Buffer; // 大缓冲区
  // 如果要往容器中存放超大对象，使用 unique_ptr 是不二选择
  typedef std::vector<std::unique_ptr<Buffer>> BufferVector;              // 大缓冲区 vector
  typedef BufferVector::value_type BufferPtr;                             // 大缓冲区 vector 元素类型

  const int flushInterval_;   // 刷新间隔
  std::atomic<bool> running_; // 原子bool类型
  const string basename_;     // 日志文件 最后的文件名
  const off_t rollSize_;      // 滚动最大值
  WJ::Thread thread_;
  WJ::CountDownLatch latch_;     // 用于等待线程启动
  WJ::MutexLock mutex_;
  WJ::Condition cond_ GUARDED_BY(mutex_);
  BufferPtr currentBuffer_ GUARDED_BY(mutex_); // 当前的缓冲区
  BufferPtr nextBuffer_ GUARDED_BY(mutex_);    // 预备缓冲区
  BufferVector buffers_ GUARDED_BY(mutex_);    // 缓冲区 vector
};

} // namespace WJ

#endif // WJ_BASE_ASYNCLOGGING_H
