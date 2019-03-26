#ifndef WJ_BASE_BLOCKINGQUEUE_H
#define WJ_BASE_BLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"

#include <deque>
#include <assert.h>

namespace WJ
{

template <typename T>
class BlockingQueue : noncopyable{
  public:
    BlockingQueue()
        : mutex_(),
          notEmpty_(mutex_),
          queue_(){
    }

    void put(const T &x){
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify(); // wait morphing saves us
    }

    void put(T &&x){
        MutexLockGuard lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify();
    }

    T take(){
        MutexLockGuard lock(mutex_);
        // always use a while-loop, due to spurious wakeup
        while (queue_.empty()){
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();
        return std::move(front);
    }

    size_t size() const{
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

  private:
    mutable MutexLock mutex_;
    // GUARDED_BY 保证对变量的使用必须要先获得锁 mutex_
    Condition notEmpty_ GUARDED_BY(mutex_);
    std::deque<T> queue_ GUARDED_BY(mutex_);
};

} // namespace WJ

#endif // WJ_BASE_BLOCKINGQUEUE_H
