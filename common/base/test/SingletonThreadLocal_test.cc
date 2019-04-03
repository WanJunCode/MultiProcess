#include "../Singleton.h"
#include "../CurrentThread.h"
#include "../ThreadLocal.h"
#include "../Thread.h"

#include <stdio.h>
#include <unistd.h>

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

#define STL WJ::Singleton<WJ::ThreadLocal<Test> >::instance().value()

void print()
{
  printf("tid=%d, %p name=%s\n",
         WJ::CurrentThread::tid(),
         &STL,
         STL.name().c_str());
}

void threadFunc(const char* changeTo)
{
  print();
  STL.setName(changeTo);
  sleep(1);
  print();
}

int main()
{
  STL.setName("main one");
  WJ::Thread t1(std::bind(threadFunc, "thread1"));
  WJ::Thread t2(std::bind(threadFunc, "thread2"));
  t1.start();
  t2.start();
  t1.join();
  print();
  t2.join();
  pthread_exit(0);
}
