#ifndef STONE_COMPILE_COMPILEALLOCATION_H
#define STONE_COMPILE_COMPILEALLOCATION_H

#include <cassert>
#include <cstddef>

namespace stone {
class CompilerInstance;
void *AllocateInCompilerInstance(size_t bytes, const CompilerInstance &compiler,
                                 mem::AllocationArena arena,
                                 unsigned alignment);
/// Types inheriting from this class are intended to be allocated in an
/// \c CompilerInstance allocator; you cannot allocate them by using a normal \c
/// new, and instead you must either provide an \c ASTContext or use a placement
/// \c new.
///
/// The template parameter is a type with the desired alignment. It is usually,
/// but not always, the type that is inheriting \c ASTAllocated.
template <typename AlignTy> class CompilerAllocation {
public:
  // Make vanilla new/delete illegal.
  void *operator new(size_t bytes) throw() = delete;
  void operator delete(void *data) throw() = delete;

  // Only allow allocation using the allocator in ASTContext
  // or by doing a placement new.
  void *
  operator new(size_t bytes, const CompilerInstance &compiler,
               mem::AllocationArena arena = mem::AllocationArena::Permanent,
               unsigned alignment = alignof(AlignTy)) {
    return stone::AllocateInCompilerInstance(bytes, compiler, arena, alignment);
  }

  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};
} // namespace stone

#endif
