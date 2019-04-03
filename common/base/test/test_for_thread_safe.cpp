#include "../Mutex.h"

namespace WJ
{

// this class is test for thread safety analysis
// guarded_by 没有什么作用，在clang编译环境下可以提示可能出现的bug
class BankAccount
{
  private:
    MutexLock mu;
    int balance GUARDED_BY(mu);

    void depositImpl(int amount)
    {
        MutexLockGuard guard(mu);
        balance += amount; // WARNING! Cannot write balance without locking mu.
    }

    // void withdrawImpl(int amount) REQUIRES(mu)
    void withdrawImpl(int amount)
    {
        MutexLockGuard guard(mu);
        balance -= amount; // OK. Caller must have locked mu.
    }

  public:
    void withdraw(int amount)
    {
        MutexLockGuard guard(mu);        
        // mu.lock();
        withdrawImpl(amount); // OK.  We've locked mu.
    }                         // WARNING!  Failed to unlock mu.

    void transferFrom(BankAccount &b, int amount)
    {
        mu.lock();
        b.withdrawImpl(amount); // WARNING!  Calling withdrawImpl() requires locking b.mu.
        depositImpl(amount);    // OK.  depositImpl() has no requirements.
        mu.unlock();
    }
};
} // namespace WJ
