// check 不可复制的基类

#ifndef WJ_BASE_NONCOPYABLE_H
#define WJ_BASE_NONCOPYABLE_H

namespace WJ
{

class noncopyable
{
public:
  noncopyable(const noncopyable &) = delete;
  void operator=(const noncopyable &) = delete;

protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

} // namespace WJ

#endif // WJ_BASE_NONCOPYABLE_H
