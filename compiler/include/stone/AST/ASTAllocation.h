#ifndef STONE_AST_ALLOCATION_H
#define STONE_AST_ALLOCATION_H

#include <cassert>
#include <cstddef>

namespace stone {
namespace ast {
class ASTContext;
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
  Temporary
};

namespace ast {
void *AllocateInASTContext(size_t bytes, const ast::ASTContext &ctx,
                              AllocationArena arena, unsigned alignment);
}

namespace ast {
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
  void *operator new(size_t bytes, const ast::ASTContext &ctx,
                     AllocationArena arena = AllocationArena::Permanent,
                     unsigned alignment = alignof(AlignTy)) {
    return ast::AllocateInASTContext(bytes, ctx, arena, alignment);
  }

  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};

} // namespace ast

} // namespace stone

#endif
