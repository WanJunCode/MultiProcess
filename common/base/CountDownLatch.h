// check 倒计时

// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef WJ_BASE_COUNTDOWNLATCH_H
#define WJ_BASE_COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"

namespace WJ
{

// 默认继承方式是 private
class CountDownLatch : noncopyable
{
 public:

  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable MutexLock mutex_;
  Condition condition_ GUARDED_BY(mutex_);
  int count_ GUARDED_BY(mutex_);
};

}  // namespace WJ
#endif  // WJ_BASE_COUNTDOWNLATCH_H
