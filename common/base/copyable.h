// check 可以复制的基类

#ifndef WJ_BASE_COPYABLE_H
#define WJ_BASE_COPYABLE_H

namespace WJ
{

/// A tag class emphasises the objects are copyable.
/// The empty base class optimization applies.
/// Any derived class of copyable should be a value type.
class copyable
{
 protected:
  copyable() = default;
  ~copyable() = default;
};

}  // namespace WJ

#endif  // WJ_BASE_COPYABLE_H
