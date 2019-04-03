#include "../ThreadLocalSingleton.h"
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

void threadFunc(const char* changeTo)
{
  printf("tid=%d, %p name=%s\n",
         WJ::CurrentThread::tid(),
         &WJ::ThreadLocalSingleton<Test>::instance(),
         WJ::ThreadLocalSingleton<Test>::instance().name().c_str());
  WJ::ThreadLocalSingleton<Test>::instance().setName(changeTo);
  printf("tid=%d, %p name=%s\n",
         WJ::CurrentThread::tid(),
         &WJ::ThreadLocalSingleton<Test>::instance(),
         WJ::ThreadLocalSingleton<Test>::instance().name().c_str());

  // no need to manually delete it
  // WJ::ThreadLocalSingleton<Test>::destroy();
}

int main()
{
  WJ::ThreadLocalSingleton<Test>::instance().setName("main one");
  WJ::Thread t1(std::bind(threadFunc, "thread1"));
  WJ::Thread t2(std::bind(threadFunc, "thread2"));
  t1.start();
  t2.start();
  t1.join();
  printf("tid=%d, %p name=%s\n",
         WJ::CurrentThread::tid(),
         &WJ::ThreadLocalSingleton<Test>::instance(),
         WJ::ThreadLocalSingleton<Test>::instance().name().c_str());
  t2.join();

  pthread_exit(0);
}
