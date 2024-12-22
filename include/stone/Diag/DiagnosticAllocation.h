#ifndef STONE_DIAG_DIAGALLOCATION_H
#define STONE_DIAG_DIAGALLOCATION_H

#include "llvm/Support/Allocator.h"

namespace stone {
template <typename AlignTy> class DiagnosticAllocation {
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
                     size_t Alignment = alignof(AlignTy)) {
    return C.Allocate(Bytes, Alignment);
  }
};

} // namespace stone

#endif