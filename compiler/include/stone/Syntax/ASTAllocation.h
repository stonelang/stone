#ifndef STONE_SYNTAXCTX_SYNTAXALLOCATION_H
#define STONE_SYNTAXCTX_SYNTAXALLOCATION_H

#include "stone/Basic/Mem.h"

#include <cassert>
#include <cstddef>

namespace stone {
namespace syn {
class ASTContext;
}

namespace syn {
void *AllocateInASTContext(size_t bytes, const syn::ASTContext &ctx,
                           mem::AllocationArena arena, unsigned alignment);
}

namespace syn {
/// Types inheriting from this class are intended to be allocated in an
/// \c ASTContext allocator; you cannot allocate them by using a normal \c
/// new, and instead you must either provide an \c ASTContext or use a placement
/// \c new.
///
/// The template parameter is a type with the desired alignment. It is usually,
/// but not always, the type that is inheriting \c ASTAllocated.
template <typename AlignTy> class ASTAllocation {
public:
  // Make vanilla new/delete illegal.
  void *operator new(size_t bytes) throw() = delete;
  void operator delete(void *data) throw() = delete;

  // Only allow allocation using the allocator in ASTContext
  // or by doing a placement new.
  void *
  operator new(size_t bytes, const syn::ASTContext &ctx,
               mem::AllocationArena arena = mem::AllocationArena::Permanent,
               unsigned alignment = alignof(AlignTy)) {
    return syn::AllocateInASTContext(bytes, ctx, arena, alignment);
  }

  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};

} // namespace syn

} // namespace stone

#endif
