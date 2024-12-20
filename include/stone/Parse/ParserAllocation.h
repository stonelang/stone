#ifndef STONE_PARSE_PARSER_ALLOCATION_H
#define STONE_PARSE_PARSER_ALLOCATION_H

#include "llvm/Support/ErrorHandling.h"

#include <stddef.h>

namespace stone {
class Parser;

template <typename T> class ParserAllocation {
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
    return C.AllocateMemory(Bytes, Alignment);
  }
};

} // namespace stone

#endif
