#ifndef STONESYNTAXCTX_ALLOC_H
#define STONESYNTAXCTX_ALLOC_H

#include <cstddef>

namespace stone {
namespace syn {
class SyntaxContext;
}
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
