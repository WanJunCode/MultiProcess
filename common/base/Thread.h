// check 对线程的封装， 使用ThreadData 完成对线程函数的封装和执行

// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef WJ_BASE_THREAD_H
#define WJ_BASE_THREAD_H

#include "Atomic.h"
#include "CountDownLatch.h"
#include "Types.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace WJ
{

class Thread : noncopyable
{
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(ThreadFunc, const string& name = string());
  // FIXME: make it movable in C++11
  ~Thread();

  void start();
  int join(); // return pthread_join()

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const string& name() const { return name_; }

  // 类 静态函数
  static int numCreated() { return numCreated_.get(); }

 private:
  void setDefaultName();

  bool       started_;
  bool       joined_;
  pthread_t  pthreadId_;
  pid_t      tid_;  // process identifications
  ThreadFunc func_;
  string     name_;
  CountDownLatch latch_;

  // 静态数据类型，用于记录创建了多少个线程
  static AtomicInt32 numCreated_;
};

}  // namespace WJ
#endif  // WJ_BASE_THREAD_H
