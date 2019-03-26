#ifndef WJ_BASE_CONDITION_H
#define WJ_BASE_CONDITION_H

#include <pthread.h>
#include "Mutex.h"

namespace WJ
{

// condition 的使用 必须结合 mutex
class Condition : noncopyable
{
public:
  explicit Condition(MutexLock &mutex)
      : mutex_(mutex)
  {
    MCHECK(pthread_cond_init(&pcond_, NULL));
  }

  ~Condition()
  {
    MCHECK(pthread_cond_destroy(&pcond_));
  }

  void wait()
  {
    // 资源控制类，构造时 清空当前 holder，析构函数中设置holder为当前线程id
    MutexLock::UnassignGuard ug(mutex_);
    MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
  }

  // returns true if time out, false otherwise.
  bool waitForSeconds(double seconds);

  void notify()
  {
    MCHECK(pthread_cond_signal(&pcond_));
  }

  void notifyAll()
  {
    MCHECK(pthread_cond_broadcast(&pcond_));
  }

private:
  MutexLock &mutex_;
  pthread_cond_t pcond_;
};

} // namespace WJ

#endif // WJ_BASE_CONDITION_H
