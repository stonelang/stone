#ifndef STONE_BASIC_ALLOCATION_H
#define STONE_BASIC_ALLOCATION_H

#include "llvm/Support/Allocator.h"

#include <cassert>
#include <cstring>
#include <memory>

namespace stone {
template <typename T> class Allocation {
public:
  /// Disable non-placement new.
  void *operator new(size_t) = delete;
  void *operator new[](size_t) = delete;

  /// Disable non-placement delete.
  void operator delete(void *) = delete;
  void operator delete[](void *) = delete;

  /// Custom version of 'new' that uses the SILModule's BumpPtrAllocator with
  /// precise alignment knowledge.  This is templated on the allocator type so
  /// that this doesn't require including SILModule.h.
  template <typename ContextTy>
  void *operator new(size_t Bytes, const ContextTy &C,
                     size_t Alignment = alignof(T)) {
    return C.Allocate(Bytes, Alignment);
  }

  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};

} // namespace stone

#endif