#ifndef STONESYNTAXCTX_ALLOC_H
#define STONESYNTAXCTX_ALLOC_H

#include <cstddef>

namespace stone {
namespace syn {
class SyntaxContext;
}
// namespace detail {
// void *AllocateInSyntaxContext(size_t bytes, const syn::SyntaxContext &ctx,
//                               AllocationArena arena, unsigned alignment);
// }
// template <typename AlignTy> class SyntaxAllocation {
// public:
//   // Make vanilla new/delete illegal.
//   void *operator new(size_t bytes) throw() = delete;
//   void operator delete(void *data) throw() = delete;

//   // Only allow allocation using the allocator in ASTContext
//   // or by doing a placement new.
//   void *operator new(size_t bytes, const syn::SyntaxContext &ctx,
//                      AllocationArena arena = AllocationArena::Permanent,
//                      unsigned alignment = alignof(AlignTy)) {

//     return detail::AllocateInSyntaxContext(bytes, ctx, arena, alignment);
//   }

//   void *operator new(size_t bytes, void *mem) throw() {
//     assert(mem && "placement new into failed allocation");
//     return mem;
//   }
// };

} // namespace stone

// Defined in SyntaxContext.h
void *operator new(size_t bytes, const stone::syn::SyntaxContext &tc,
                   size_t alignment = 8);
//
void *operator new[](size_t bytes, const stone::syn::SyntaxContext &tc,
                     size_t alignment = 8);
// It is good practice to pair new/delete operators.  Also, MSVC gives many
// warnings if a matching delete overload is not declared, even though the
// throw() spec guarantees it will not be implicitly called.
void operator delete(void *Ptr, const stone::syn::SyntaxContext &tc, size_t);
//
void operator delete[](void *Ptr, const stone::syn::SyntaxContext &tc, size_t);

#endif
