#include "../Singleton.h"
#include "../CurrentThread.h"
#include "../Thread.h"

#include <stdio.h>

class Test : WJ::noncopyable
{
 public:
  Test()
  {
    printf("tid=%d, constructing %p\n", WJ::CurrentThread::tid(), this);
  }

  ~Test()
  {
    printf("tid=%d, destructing %p %s\n", WJ::CurrentThread::tid(), this, name_.c_str());
  }

  const WJ::string& name() const { return name_; }
  void setName(const WJ::string& n) { name_ = n; }

 private:
  WJ::string name_;
};

class TestNoDestroy : WJ::noncopyable
{
 public:
  // Tag member for Singleton<T>
  void no_destroy();

  TestNoDestroy()
  {
    printf("tid=%d, constructing TestNoDestroy %p\n", WJ::CurrentThread::tid(), this);
  }

  ~TestNoDestroy()
  {
    printf("tid=%d, destructing TestNoDestroy %p\n", WJ::CurrentThread::tid(), this);
  }
};

void threadFunc()
{
  printf("tid=%d, %p name=%s\n",
         WJ::CurrentThread::tid(),
         &WJ::Singleton<Test>::instance(),
         WJ::Singleton<Test>::instance().name().c_str());
  WJ::Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
  WJ::Singleton<Test>::instance().setName("only one");
  WJ::Thread t1(threadFunc);
  t1.start();
  t1.join();
  printf("tid=%d, %p name=%s\n",
         WJ::CurrentThread::tid(),
         &WJ::Singleton<Test>::instance(),
         WJ::Singleton<Test>::instance().name().c_str());
  WJ::Singleton<TestNoDestroy>::instance();
  printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}
