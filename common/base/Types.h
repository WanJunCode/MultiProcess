// check 主要讲解了许多 C++ 安全类型转换的技巧

#ifndef WJ_BASE_TYPES_H
#define WJ_BASE_TYPES_H

#include <stdint.h>
#include <string.h>  // memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

///
/// The most common stuffs.
///
namespace WJ
{

using std::string;

// 简化了 memset 的使用
inline void memZero(void* p, size_t n)
{
  memset(p, 0, n);
}

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers
{
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
  return static_cast<To>(f);
}

}  // namespace WJ

#endif  // WJ_BASE_TYPES_H
