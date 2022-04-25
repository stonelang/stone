#ifndef STONE_SYNTAXCTX_SYNTAXALLOCATION_H
#define STONE_SYNTAXCTX_SYNTAXALLOCATION_H

#include <cassert>
#include <cstddef>

namespace stone {
namespace syn {
class SyntaxContext;
}
/// The arena in which a particular ASTContext allocation will go.
enum class AllocationArena {
  /// The permanent arena, which is tied to the lifetime of
  /// the ASTContext.
  ///
  /// All global declarations and types need to be allocated into this arena.
  /// At present, everything that is not a type involving a type variable is
  /// allocated in this arena.
  Permanent,
  /// The constraint solver's temporary arena, which is tied to the
  /// lifetime of a particular instance of the constraint solver.
  ///
  /// Any type involving a type variable is allocated in this arena.
  ConstraintSolver
};

namespace syn {
void *AllocateInSyntaxContext(size_t bytes, const syn::SyntaxContext &ctx,
                              AllocationArena arena, unsigned alignment);
}

namespace syn {
/// Types inheriting from this class are intended to be allocated in an
/// \c ASTContext allocator; you cannot allocate them by using a normal \c new,
/// and instead you must either provide an \c ASTContext or use a placement
/// \c new.
///
/// The template parameter is a type with the desired alignment. It is usually,
/// but not always, the type that is inheriting \c ASTAllocated.
template <typename AlignTy> class SyntaxAllocation {
public:
  // Make vanilla new/delete illegal.
  void *operator new(size_t bytes) throw() = delete;
  void operator delete(void *data) throw() = delete;

  // Only allow allocation using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(size_t bytes, const syn::SyntaxContext &ctx,
                     AllocationArena arena = AllocationArena::Permanent,
                     unsigned alignment = alignof(AlignTy)) {
    return syn::AllocateInSyntaxContext(bytes, ctx, arena, alignment);
  }

  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};

} // namespace syn

} // namespace stone

#endif
